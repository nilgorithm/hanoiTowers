#include <gtk/gtk.h>
#include <cairo.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

static const int w_poll_height = 200;
static const int w_height = 700;
static const int w_width = 500;
static const int w_l_width = 10;
static int count[] = {0, 0, 0};
static int max = 10;
static int max_step = 0;
static int step = 0;
static int timeout = 500;

struct All
{
	GtkWidget *widget;
	cairo_t *cr;
	GtkSpinButton *spin;
};


enum position
{
	left,
	central,
	right
};

static enum position gl_from, gl_to; 

struct Step
{
	int value;
	enum position to;
};

static struct Step *steps = NULL;

struct Rect
{
	int height;
	int value;
	enum position p;
};

static struct Rect *rects = NULL;

void move_disk(int n, enum position from, enum position to, struct All *data);

void towerOfHanoi(int n, enum position from_rod, enum position to_rod, enum position aux_rod, struct All *data)
{
    if (n == 1)
    {
        printf("\n Move disk 1 from rod %c to rod %c", from_rod, to_rod);
		move_disk(n, from_rod, to_rod, data);
        return;
    }
    towerOfHanoi(n-1, from_rod, aux_rod, to_rod, data);
    printf("\n Move disk %d from rod %c to rod %c", n, from_rod, to_rod);
	move_disk(n, from_rod, to_rod, data);
    towerOfHanoi(n-1, aux_rod, to_rod, from_rod, data);
}

int wc(enum position pos) {
	int w_c = w_width / 4;
	switch (pos) {
		case left:
			w_c = w_c / 2;
			break;
		case central:
			w_c = w_c * 2;
			break;
		case right:
			w_c = w_c * 4 - w_c / 2;
			break;
	}
	return w_c;
}


static void draw_line (cairo_t *cr, enum position from, enum position to)
{
	int left;
	int right;
	int top = w_poll_height / 3;
	if (wc(from) < wc(to)) {
		left = wc(from) - w_l_width / 2;
		right = wc(to) + w_l_width / 2;
	} else {
		left = wc(to) - w_l_width / 2;
		right = wc(from) + w_l_width / 2;
	}
	cairo_rectangle (cr, left, top, right-left, w_l_width);
	cairo_set_source_rgb (cr, 0.7, 0.7, 0.7);   /* set fill color */
	cairo_fill (cr);

	cairo_rectangle (cr, left, top, w_l_width, top);
	cairo_set_source_rgb (cr, 0.7, 0.7, 0.7);   /* set fill color */
	cairo_fill (cr);

	cairo_rectangle (cr, right - w_l_width, top, w_l_width, top);
	cairo_set_source_rgb (cr, 0.7, 0.7, 0.7);   /* set fill color */
	cairo_fill (cr);

	cairo_move_to(cr, wc(to) - w_l_width * 3, top * 2);
	cairo_line_to(cr, wc(to) + w_l_width * 3, top * 2);
	cairo_line_to(cr, wc(to), top * 2 + top / 2);
	cairo_fill (cr);
}

static void draw_rect (cairo_t *cr, struct Rect* rect)
{
	if (!rect || !cr)
		return ;
	int w = w_width / 4;
	int h = (w_height - w_poll_height) / (2 * max + 2);
	int w_c = wc(rect->p);
	//printf("w_c %d\n", w_c);
	w = w * (rect->value + 1) / max;
	cairo_rectangle (cr, w_c - w / 2, 2 * h * rect->height + w_poll_height, w, h);     /* set rectangle */
	if (rect->value & 1)
		cairo_set_source_rgb (cr, 0, 0, 1);   /* set fill color */
	else
		cairo_set_source_rgb (cr, 1, 0, 0);   /* set fill color */
	cairo_fill (cr);                            /* fill rectangle */
}

static void draw(cairo_t *cr)
{	
	cairo_rectangle (cr, 0, 0, w_width, w_height);     /* set rectangle */
	cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);   /* set fill color */
	cairo_fill (cr);                            /* fill rectangle */
	for (int i = 0; i < 3; i++) {
		cairo_rectangle (cr, wc(i) - w_l_width/2, w_poll_height, w_l_width, w_height - w_poll_height);     /* set rectangle */
		cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);   /* set fill color */
		cairo_fill (cr);                            /* fill rectangle */
	}
	if (!rects)
		return ;
	struct Rect r;
	for (int i = 0; i < max; i++) {
		r = rects[i];
		//r.value = i;
		//r.height = i;
		//r.p = left;
		//if (count > i) {
			//draw_line(cr, 0, 2);
			//r.p = right;
		//}
		draw_rect (cr, &r);     /* draw rectangle in window */
	}
}

static void redraw(GtkWidget *widget, cairo_t *cr)
{
	//GtkWidget* widget = (GtkWidget*) cr;
	//draw(cr);
	gtk_widget_queue_draw (widget);
	gtk_widget_queue_draw ((GtkWidget*) cr);
}

void move_disk(int n, enum position from, enum position to, struct All *data)
{
	if (!rects || !steps)
		return ;
	//rects[max-n].p = to;
	//redraw(data->widget, data->cr);
	//sleep(1);
	//gtk_widget_show(data->widget);
	steps[step].value = n-1;
	steps[step].to = to;
	printf("%d", step++);
}

static gboolean on_draw_event (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	draw(cr);
	draw_line(cr, gl_from, gl_to);

	return FALSE;
	(void)user_data, (void)widget;  /* suppress -Wunused warning */
}

static void
print_hello (GtkWidget *widget, gpointer data)
{
	g_print ("Hello World\n");
}

gboolean
time_handler(struct All *data)
{
	if (step >= max_step)
		return FALSE;
	int v = steps[step].value;
	enum position from = rects[v].p;
	enum position to = steps[step].to;
	gl_from = from;
	gl_to = to;
	printf("%d/%d, [%d] %d -> %d\n", step, max_step, v, (int)from, (int)to);
  	rects[v].p = to;
	count[(int)from]--;
	count[(int)to]++;
	rects[v].height = max - count[(int)rects[v].p];
	draw_line(data->cr, left, right);
	//draw(data->cr);
	//draw_line(data->cr, from, to);
	draw_line(data->cr, left, right);

	gtk_widget_queue_draw (data->widget);
	gtk_widget_queue_draw ((GtkWidget*) data->cr);
	step++;

	//if (step < max_step)
		//g_timeout_add(timeout, (GSourceFunc) time_handler, data);
	return TRUE;
}

static void
start(GtkWidget *widget, struct All *all)
{
	max = gtk_spin_button_get_value_as_int(all->spin);
	count[0] = max;
	for (int i = 1; i < 3; i++)
		count[i] = 0;
	//redraw(all->widget, all->cr);
	if (rects)
		free(rects);
	rects = (struct Rect*)malloc(max*sizeof(struct Rect));
	for (int i = 0; i < max; i++) {
		rects[i].value = i;
		rects[i].height = i;
		rects[i].p = left;
	}
	max_step = pow(2, 2*max);
	step = 0;
	if (steps)
		free(steps);
	steps = (struct Step*)malloc(max_step*sizeof(struct Step));
	towerOfHanoi(max, left, right, central, all);
	max_step = step;
	step = 0;
	g_timeout_add(timeout, (GSourceFunc) time_handler, all);
}




int
main (int argc, char **argv)
{
#if 0
	GtkApplication *app;
	int status;

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	//g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
#endif
	GtkWidget *window;
	GtkWidget *grph_window;
	GtkWidget *darea;           /* cairo drawing area */
	GtkSpinButton *spin;

	struct All data;

	gtk_init (&argc, &argv);    /* required with every gtk app */

	//window = gtk_application_window_new (app);
	grph_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);  
	data.widget = grph_window;
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);  

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(window), hbox);

	darea = gtk_drawing_area_new();                 /* create cairo area */
	data.cr = (cairo_t *)darea;
	gtk_container_add (GTK_CONTAINER(grph_window), darea);   
	//gtk_box_pack_start(GTK_BOX(hbox), darea, TRUE, TRUE, 0);

	/* connect callbacks to draw rectangle and close window/quit app */
	g_signal_connect (G_OBJECT(darea), "draw",
	G_CALLBACK(on_draw_event), NULL);
	g_signal_connect (G_OBJECT(window), "destroy",
	G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT(grph_window), "destroy",
	G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *button;
	GtkWidget *button_box;

	//window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Window");
	gtk_window_set_default_size (GTK_WINDOW (grph_window), w_width, w_height);

	button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
	//gtk_container_add (GTK_CONTAINER (window), button_box);
	gtk_box_pack_start(GTK_BOX(hbox), button_box, TRUE, TRUE, 0);

	button = gtk_button_new_with_label ("Hello World");
	g_signal_connect (button, "clicked", G_CALLBACK (redraw), darea);
	gtk_container_add (GTK_CONTAINER (button_box), button);

	button = gtk_button_new_with_label ("Start");
	g_signal_connect (button, "clicked", G_CALLBACK (start), &data);
	gtk_container_add (GTK_CONTAINER (button_box), button);

	spin = (GtkSpinButton *)gtk_spin_button_new_with_range(1.0, 20.0, 1.0);
	gtk_spin_button_set_value(spin, 10);
	gtk_container_add (GTK_CONTAINER (button_box), (GtkWidget *)spin);
	data.spin = spin;

	gtk_widget_show_all (grph_window);   /* show all windows */
	gtk_widget_show_all (window);   /* show all windows */

	gtk_main();     /* pass control to gtk event-loop */

#if 0
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
#endif
	return 0;
}

