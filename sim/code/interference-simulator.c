#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <png.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>

#define max(a, b) ((a)<(b)?(b):(a))
#define min(a, b) ((a)<(b)?(a):(b))

#define EPSILON 0.00001
#define APP_HEIGHT 10

int print_steps = 0;

int rand_start(int min, int max) {
    return (random() % (max-min+1)) + min;
}

static void setRGB(png_byte *ptr, float val)
{
    if(val == 0.0) {
        ptr[0] = 255;
        ptr[1] = 255;
        ptr[2] = 255;
    } else {
        if( val < 0.5 ) {
            ptr[0] = 255;
            ptr[1] = 255;
            ptr[2] = (int)(2.0*(0.5-val)*255.0);
        } else {
            ptr[0] = (int)(2.0*(val-0.5)*255.0);
            ptr[1] = (int)(2.0*(1.0-val)*255.0);
            ptr[2] = (int)(2.0*(1.0-val)*255.0);
        }
    }
}

typedef struct app {
    struct app *next;
    int      size;
    int      class;
    double   start; /* Date of start */
    double   end;   /* Date of end   */
    double   T;     /* Checkpoint period */
    double   input; /* Time (without interference) spent to input */
    double   output;/* Time (without interference) spent to output */
    double   csize; /* Time (without interference) spent to checkpoint */
    double   iosize;/* Time (without interference) spent during the run between start and end doing analysis I/O */
} app_t;

typedef struct time_step {
    struct time_step *next;
    app_t *app;
    double value;
} time_step_t;

static time_step_t *time_step_freelist = NULL;

void time_step_free(time_step_t *elt)
{
    elt->next = time_step_freelist;
    time_step_freelist = elt;
}

time_step_t *time_step_malloc(void)
{
    time_step_t *ts;
    if( NULL == time_step_freelist )
        ts = malloc(sizeof(time_step_t));
    else {
        ts = time_step_freelist;
        time_step_freelist = ts->next;
    }
    memset(ts, 0, sizeof(time_step_t));
    return ts;
}

double SIM_GET(time_step_t **sim, uint64_t t, app_t *a)
{
    time_step_t *s;
    for(s = sim[t]; s!=NULL; s = s->next) {
        if(a == s->app)
            return s->value;
    }
    return 0.0;
}

void SIM_SET(time_step_t **sim, uint64_t t, app_t *a, double v)
{
    time_step_t *s, *prev = NULL;
    assert(NULL != a);
    for(s = sim[t]; s!=NULL; s = s->next) {
        if(a == s->app) {
            if( 0.0 == v ) {
                if(NULL == prev)
                    sim[t] = s->next;
                else
                    prev->next = s->next;
                time_step_free(s);
            } else {
                s->value = v;
            }
            return;
        }
        prev=s;
    }
    if( 0.0 == v )
        return;
    s = time_step_malloc();
    s->next = sim[t];
    s->app = a;
    s->value = v;
    sim[t] = s;
}

void SIM_ADD(time_step_t **sim, uint64_t t, app_t *a, double v)
{
    time_step_t *s;
    assert(NULL != a);
    for(s = sim[t]; s!=NULL; s = s->next) {
        if(a == s->app) {
            s->value += v;
            return;
        }
    }
    s = time_step_malloc();
    s->next = sim[t];
    s->app = a;
    s->value = v;
    sim[t] = s;
}

void sim_free(time_step_t **sim, uint64_t T)
{
    time_step_t *s, *n;
    uint64_t t;
    for(t = 0; t < T; t++) {
        for(s = sim[t]; s!=NULL; s = n) {
            n=s->next;
            time_step_free(s);
        }
    }
}

static int output_sim(char *filename, int nb_app, app_t *apps, uint64_t T, double delta, time_step_t **sim, int step)
{
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    int width = T;
    int height = nb_app*APP_HEIGHT;
    app_t *a;
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "#Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }
    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "#Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "#Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }
    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "#Error during png creation\n");
        code = 1;
        goto finalise;
    }
    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    png_text title_text;
    title_text.compression = PNG_TEXT_COMPRESSION_NONE;
    title_text.key = "Title";
    asprintf(&title_text.text, "Simulation at step %d", step);
    png_set_text(png_ptr, info_ptr, &title_text, 1);
    free(title_text.text);

    png_write_info(png_ptr, info_ptr);
    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * width * sizeof(png_byte));

    // Write image data
    int x, y, i, p;
    for(a = apps, p=0; a != NULL; p++, a = a->next) {
        for (i=0; i<T ; i++) {
            x = i;
            if( SIM_GET(sim, i, a) == 0.0 ) {
                setRGB(&(row[x*3]), 0.0);
            } else {
                setRGB(&(row[x*3]), SIM_GET(sim, i, a)/delta );
            }
        }
        for (y=APP_HEIGHT*p ; y<APP_HEIGHT*(p+1); y++) {
            png_write_row(png_ptr, row);
        }
    }
   
    // End write
    png_write_end(png_ptr, NULL);
 finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}

static double interference_model(int app_size, int load)
{
    return (double)app_size/(double)load;
}

static double sim_one(int nb_app, app_t *apps, uint64_t T, double delta, time_step_t **sim)
{
	int p, i, load, loop, nio;
    uint32_t s, e, io, t, tis;
    char filename[512];
    double ckpt_time, ckpt_intv, to_checkpoint;
    double already_used, checkpointed, resource_io_withinterference, amount;
    double resource_io_nointerference = 0.0;
    app_t *a;
    time_step_t *ts, *next_ts;
        
    memset(sim, 0, sizeof(time_step_t *)*(T+1));
    for(p = 0, a = apps; NULL != a; p++, a = a->next) {
        s = (uint32_t)floor(a->start / delta);
        e = (uint32_t)floor(a->end / delta);
        if( a->input > 0.0 ) {
            for(t = s; t < s + (uint32_t)floor(a->input/delta); t++) {
                SIM_ADD(sim, t, a, delta);
            }
            resource_io_nointerference += a->input * a->size;
        }
        if( a->output > 0.0 ) {
            for(t = e - (uint32_t)floor(a->output/delta); t < e; t++)
                SIM_ADD(sim, t, a, delta);
            resource_io_nointerference += a->output * a->size;
        }
        s += (uint32_t)floor(a->input/delta);
        e -= (uint32_t)floor(a->output/delta);
        if( e <= s) {
            fprintf(stderr, "#For app %d, all time spent doing I/O for input and output\n", p);
            sim_free(sim, T+1);
            return -1.0;
        }
        if( a->iosize > 0.0 ) {
            nio = (uint64_t)floor(a->iosize/delta);
            for(io = s; io < e; io += (e-s)/nio)
                SIM_ADD(sim, io, a, delta);
            resource_io_nointerference += a->iosize * a->size;
        }
        ckpt_time = (uint32_t)floor(a->csize / delta);
        ckpt_intv = (uint32_t)floor(a->T / delta);
        if( ckpt_intv <= ckpt_time ) {
            fprintf(stderr, "#For app %d, all time spent checkpointing\n", p);
            sim_free(sim, T+1);
            return -1.0;
        }
        for(t = s + ckpt_intv; t < e-ckpt_time; t += ckpt_intv) {
            to_checkpoint = a->csize;
            resource_io_nointerference += a->csize * a->size;
            tis = 0;
            while( to_checkpoint > 0.0 ) {
                already_used = SIM_GET(sim, t+tis, a);
                checkpointed = min(to_checkpoint, delta-already_used);
                SIM_ADD(sim, t+tis, a, checkpointed);
                to_checkpoint -= checkpointed;
                tis++;
            }
        }
    }

    loop = 0;
    if(print_steps) {
        snprintf(filename, 512, "step%03d.png", loop);
        if( output_sim(filename, nb_app, apps, T, delta, sim, loop) ) {
            fprintf(stderr, "#creating %s failed\n", filename);
        }
    }
    do {
        loop++;
        resource_io_withinterference = 0.0;
        
        for(i = 0; i < T; i++) {
            load = 0;
            amount = 0.0;
            for(ts = sim[i]; NULL != ts; ts = ts->next) {
                load += ts->app->size;
                amount += ts->value;
                resource_io_withinterference += delta * ts->app->size;
            }
            if( amount > delta ) {
                double before;
                double time_transferred;
                for(ts = sim[i]; NULL != ts; ts = ts->next) {
                    before = ts->value;
                    time_transferred = min(delta*interference_model(ts->app->size, load), before);
                    SIM_ADD(sim, i+1, ts->app, before-time_transferred);
                    ts->value = time_transferred;
                }
            }
        }
        
        load = 0;
        for(ts = sim[T]; NULL != ts; ts = next_ts) {
            if( ts->value != SIM_GET(sim, 0, ts->app) ) {
                SIM_SET(sim, 0, ts->app, ts->value);
                load++;
            }
            next_ts = ts->next;
            time_step_free(ts);
        }
        sim[T] = NULL;

        if( print_steps ) {
            snprintf(filename, 512, "step%03d.png", loop);
            if( output_sim(filename, nb_app, apps, T, delta, sim, loop) ) {
                fprintf(stderr, "#creating %s failed\n", filename);
            }
        }
        fprintf(stderr, "#Step %d, resource_io_withinterference = %g, ratio = %g\n",
                loop, resource_io_withinterference, resource_io_withinterference / resource_io_nointerference);
        if(loop > 50) {
          fprintf(stderr, "#Reached max convergence\n");
          sim_free(sim, T+1);
          return -1.0;
        }
        if( resource_io_withinterference / resource_io_nointerference >= 10.0 ) {
          fprintf(stderr, "#Considering divergence\n");
          break;
        }
    }  while(load > 0);
    sim_free(sim, T+1);
    return resource_io_withinterference / resource_io_nointerference;
}

typedef struct plat_hist {
    struct plat_hist *next;
    int               nodes_avail;
    double            date;
} plat_hist_t;

typedef struct {
    int nodes;
    double wall;
    double weight_goal;
    double weight;
    double ckpt;
    double output;
    double input;
    double iosize;
} app_class_t;

int app_fits(plat_hist_t *s, app_class_t *c)
{
    double ed = s->date + c->wall;
    while((s->next!=NULL) && (s->date < ed)) {
        if( s->nodes_avail < c->nodes )
            return 0;
        s = s->next;
    }
    if(NULL == s->next) {
        assert(s->nodes_avail > c->nodes);
        return 1;
    }
    return s->nodes_avail >= c->nodes;
}

void sim_init(int nb_app_class, app_class_t *classes, double bw, double mtbf, int nodes, int *_nb_apps, app_t **_apps)
{
    plat_hist_t *status, *s, *ns;
    int ac;
    double sd, ed;
    int nb_apps = 0;
    app_t *apps = NULL, *a, *next;
    double weight_sum, weight;
    double coin;
    
    status = malloc(sizeof(plat_hist_t));
    status->next = NULL;
    status->nodes_avail = nodes;
    status->date = 0.0;
    
    do {
        
        if(nb_apps > 500) {
            for(a = apps; NULL != a; a = next) {
                next = a->next;
                free(a);
            }
            nb_apps = 0;
            apps = NULL;
            for(s = status; s != NULL; s = ns) {
                ns = s->next;
                free(s);
            }
            status = malloc(sizeof(plat_hist_t));
            status->next = NULL;
            status->nodes_avail = nodes;
            status->date = 0.0;
        }
        
        weight_sum = 0.0;
        weight = 0.0;
        for(ac = 0; ac < nb_app_class; ac++) {
            weight_sum += classes[ac].weight_goal;
            classes[ac].weight = weight;
            weight = weight_sum;
        }

        coin = (weight_sum * (double)random()) / (double)RAND_MAX;
        for(ac = 0; ac < nb_app_class-1; ac++) {
            if( coin >= classes[ac].weight && coin < classes[ac+1].weight )
                break;
        }

        /* First fit ac */
        for(s = status; s!=NULL; s = s->next) {
            if( app_fits(s, &classes[ac]) ) {
                sd = s->date;
                ed = s->date + classes[ac].wall;
                while( (s->next != NULL) && (s->next->date <= ed) ) {
                    s->nodes_avail -= classes[ac].nodes;
                    s = s->next;
                }
                if( ed != s->date ) {
                    ns = malloc(sizeof(plat_hist_t));
                    ns->next = s->next;
                    ns->nodes_avail = s->nodes_avail;
                    ns->date = ed;
                    s->next = ns;
                    s->nodes_avail -= classes[ac].nodes;
                }
                a = malloc(sizeof(app_t));
                a->next = apps;
                a->size = classes[ac].nodes;
                a->start = sd;
                a->end   = ed;
                a->csize = classes[ac].ckpt;
                a->T     = sqrt(2.0 * mtbf / a->size * a->csize);
                a->input = classes[ac].input;
                a->output = classes[ac].output;
                a->iosize = classes[ac].iosize;
                a->class  = ac;
                apps = a;
                nb_apps++;
                break;
            }
        }

        for(ac = 0; ac < nb_app_class; ac++)
            classes[ac].weight = 0.0;
        weight_sum = 0.0;
        for(a = apps; NULL != a; a = a->next) {
            ac = a->class;
            weight = classes[ac].nodes * classes[ac].wall;
            classes[ac].weight += weight;
            weight_sum += weight;
        }
        for(ac = 0; ac < nb_app_class; ac++) {
            int ok;
            ok = (classes[ac].weight / weight_sum < classes[ac].weight_goal + 0.01) &&
                (classes[ac].weight / weight_sum > classes[ac].weight_goal - 0.01) &&
                nb_apps > 100;
            if( ok )
                continue;
            break;
        }
    } while( ac != nb_app_class );

    weight = 0.0;
    for(s = status; s->next != NULL; s = s->next) {
        weight += s->nodes_avail * (s->next->date - s->date);
    }
    weight_sum = s->date*s->nodes_avail;
    
    fprintf(stderr, "#%d apps: ", nb_apps);
    for(ac = 0; ac < nb_app_class; ac++) {
        fprintf(stderr, " %4.2g ", classes[ac].weight / weight_sum);
    }
    fprintf(stderr, " (resource usage ratio: %g)\n", (weight_sum - weight)/weight_sum);

    *_apps = apps;
    *_nb_apps = nb_apps;

    for(s = status; s != NULL; s = ns) {
        ns = s->next;
        free(s);
    }
}

int main(int argc, char *argv[])
{
    int nb_app = 0;
    int print = 0;
    time_step_t **sim = NULL;
    double delta;
    double mtbf = 60.0*24.0*365.25*25.0; /* 25 years in minute */
    double bw   = 60*150.0e9;            /* 150 GByte/s in GByte/minute */
    int nodes = 18860;                   /* Total number of nodes */
    double node_memory = 32e9;           /* 32GByte / node */
    int core_per_node = 16;
    int c, ac, nb_app_class = 4;
    char *m;
    int N = 1000, run;
    uint64_t T, prev_T = 0;
    app_t *apps, *a, *next;
    double Tt, r;
    int failed = 0;
    int success = 0;
    double weight_sum, min_time;

    app_class_t classes[4] = {
        {
            /* EAP */
            .nodes = 16384/core_per_node,
            .wall = 262.4*60,
            .weight_goal = 0.6,
            .weight = 0.0,
            .ckpt = 0.3,
            .output = 1.7,
            .input = 0.0,
            .iosize =  0.0
        },
        {
            /* LAP */
            .nodes = 4096/core_per_node,
            .wall = 64.0*60,
            .weight_goal = 0.05,
            .weight = 0.0,
            .ckpt = 0.75,
            .output = 1.7,
            .input = 0.0,
            .iosize =  0.0
        },
        {
            /* SILVERTON */
            .nodes = 32768/core_per_node,
            .wall = 128.0*60,
            .weight_goal = 0.15,
            .weight = 0.0,
            .ckpt = 2.1,
            .output = 1.0,
            .input = 0.7,
            .iosize = 0.05
        },
        {
            /* VPIC */
            .nodes = 30000/core_per_node,
            .wall = 157.2*60,
            .weight_goal = 0.1,
            .weight = 0.0,
            .ckpt = 0.1875,
            .output = 1.15,
            .input = 0.05,
            .iosize = 2.0
        }
    };

    srandom(getpid());

    while (1) {
        static struct option long_options[] = {
            {"bw",        required_argument, 0,  'b' },
            {"mtbf",      required_argument, 0,  'm' },
            {"N",         required_argument, 0,  'N' },
            {"png",             no_argument, 0,  'p' },
            {"print-steps",     no_argument, 0,  'P' },
            {0,         0,                   0,   0  }
        };

       c = getopt_long(argc, argv, "b:m:N:pPa:", long_options, NULL);
       if (c == -1)
           break;

       switch (c) {
       case 'b':
           bw = strtod(optarg, &m) * 60.0;
           if( (bw <= 0.0) || (*m != '\0') ) {
               fprintf(stderr, "%s is not a correct bw definition (see -h)\n",
                           optarg);
                   exit(1);
           }
           break;

       case 'm':
           mtbf = strtod(optarg, &m);
           if( (mtbf <= 0.0) || (*m != '\0') ) {
               fprintf(stderr, "%s is not a correct mtbf definition (see -h)\n",
                           optarg);
               exit(1);
           }
           break;
       case 'p':
           print = !print;
           break;

       case 'P':
           print_steps = !print_steps;
           break;

       case 'N':
           N = atoi(optarg);
           break;
           
       case '?':
       case ':':
       default:
           fprintf(stderr, "Usage: %s [options]\n"
                   "Where options are:\n"
                   "   --bw|-b   <bw>          Redefine the bandwidth (%g GByte/s)\n"
                   "   --mtbf | -m <MTBF>      Mean time between failures (to compute Daly's formula, defaults to %g minutes)\n"
                   "   --N |-N   <number of runs>  (defaults %d)\n"
                   "   --png|-p  produces a file named 'output.png' in the CWD, that represents at which speed each\n"
                   "              checkpoint ran. Implies -N 1 (default: don't print)\n"
                   "  --print-steps|-P produces a set of files named 'stepxxx.png' one per simulation step\n",
                   argv[0], bw/60.0, mtbf, N);
           exit(0);
        }
    }

    if( print ) {
        N = 1;
    }

    weight_sum = 0.0;
    min_time = -1.0;
    for(ac = 0; ac < nb_app_class; ac++) {
        classes[ac].weight_goal = classes[ac].weight_goal * classes[ac].nodes * classes[ac].wall;
        weight_sum += classes[ac].weight_goal;
        classes[ac].ckpt   = classes[ac].ckpt * classes[ac].nodes * node_memory/bw;
        if(ac == 0) min_time = classes[ac].ckpt;
        else if(classes[ac].ckpt < min_time) min_time = classes[ac].ckpt;
        classes[ac].output = classes[ac].output * classes[ac].nodes * node_memory/bw;
        if(classes[ac].output > 0.0 && classes[ac].output < min_time) min_time = classes[ac].output;
        classes[ac].input = classes[ac].input * classes[ac].nodes * node_memory/bw;
        if(classes[ac].input > 0.0 && classes[ac].input < min_time) min_time = classes[ac].input;
        classes[ac].iosize = classes[ac].iosize * classes[ac].nodes * node_memory/bw;
        if(classes[ac].iosize > 0.0 && classes[ac].iosize < min_time) min_time = classes[ac].iosize;
    }
    for(ac = 0; ac < nb_app_class; ac++) {
        classes[ac].weight_goal = classes[ac].weight_goal / weight_sum;
    }
    for(run = 0; run < N; run++) {
        sim_init(nb_app_class, classes, bw, mtbf, nodes, &nb_app, &apps);
        Tt = 0.0;
        for(a = apps; NULL != a; a = a->next) {
            if( a->end > Tt )
                Tt = a->end;
        }
        delta = min_time/20.0;
        T = (uint64_t)ceil(Tt/delta);
        fprintf(stderr, "#Using a delta of %g, this gives a T of %lu\n", delta, T);
        if( T > prev_T ) {
            sim = realloc(sim, sizeof(time_step_t*)*(T+1));
            prev_T = T;
        }      
        if( (r = sim_one(nb_app, apps, T, delta, sim)) < 0 ) {
            failed++;
        } else {
            success++;
        }
        fprintf(stderr, "%g,%g,%d,%g\n", bw/60.0, mtbf, run, r);
        for(a = apps; NULL != a; a = next) {
            next = a->next;
            free(a);
        }
    }
}
