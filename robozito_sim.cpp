// robozito_sim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"
#include <iostream>
#include <map>

const float LED_RADIUS = 0.25;
const float MEASURE_RANGE = 0.5;

enum Color {
	PRETO,
	BRANCO
};

float cm_to_pixels(float cm) {
	return cm * 20;
}

class PVector {
public:
	PVector(float _x, float _y);
	PVector(float angle);
	PVector();

	float heading();
	float mag();
	float x;
	float y;
};

PVector::PVector(float _x, float _y) :
	x(_x), y(_y) {

}

PVector::PVector(float angle) {
	x = cos(angle * ALLEGRO_PI / 180);
	y = sin(angle * ALLEGRO_PI / 180);
}

PVector::PVector() : x(0), y(0) {

}

float PVector::heading() {
	//std::cout << atan(y / x) / ALLEGRO_PI * 180 << " ";//float(x ? atan(y / x) : (y > 0 ? 90 : -90));
	return atan(y / x) / ALLEGRO_PI * 180;
}

float PVector::mag() {
	return sqrt(x * x * y * y);
}

class Sensor {
public:
	Sensor(PVector distance_from_center);
	Sensor();
	PVector dist;
	Color color;
};

Sensor::Sensor(PVector distance_from_center) :
	dist(distance_from_center) {

}

Sensor::Sensor() : dist(PVector(0)) {

}

//void Sensor::add_sensor(std::string name, PVector distance_from_center) {
//	sensores.insert(std::pair<std::string, Sensor>(name, distance_from_center));
//}

class Robo {
public:
	Robo(std::map<std::string, Sensor> ss, float x, float y, float ang);
	~Robo();

	void update();
	void draw();
private:
	std::map<std::string, Sensor> sensores;
	ALLEGRO_BITMAP *sensores_img;
	PVector pos;
	PVector vel;
	PVector center;
};

Robo::Robo(std::map<std::string, Sensor> ss, float x, float y, float ang) :
	sensores(ss), pos(PVector(x, y)), vel(PVector(ang)), center(PVector(0)) {
	float smallest_x = (sensores.begin())->second.dist.x, 
		  smallest_y = (sensores.begin())->second.dist.y,
		  greatest_x = (sensores.begin())->second.dist.x,
		  greatest_y = (sensores.begin())->second.dist.y;

	for (std::map<std::string, Sensor>::iterator i = sensores.begin(); i != sensores.end(); ++i) {
		if (i->second.dist.x < smallest_x) {
			smallest_x = i->second.dist.x;
		} else if (i->second.dist.x > greatest_x) {
			greatest_x = i->second.dist.x;
		}
		if (i->second.dist.y < smallest_y) {
			smallest_y = i->second.dist.y;
		} else if (i->second.dist.y > greatest_y) {
			greatest_y = i->second.dist.y;
		}
	}

	center = PVector(-cm_to_pixels(smallest_x - MEASURE_RANGE), cm_to_pixels(greatest_y + MEASURE_RANGE));
	sensores_img = al_create_bitmap(cm_to_pixels(greatest_x - smallest_x + 2 * MEASURE_RANGE),
									cm_to_pixels(greatest_y - smallest_y + 2 * MEASURE_RANGE));
	if (!sensores_img) {
		throw;
	}

	ALLEGRO_BITMAP *old = al_get_target_bitmap();
	al_set_target_bitmap(sensores_img);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	ALLEGRO_TRANSFORM tf;
	al_identity_transform(&tf);
	al_translate_transform(&tf, center.x, center.y);
	al_use_transform(&tf);
	for (std::map<std::string, Sensor>::iterator i = sensores.begin(); i != sensores.end(); ++i) {
		al_draw_filled_circle(cm_to_pixels(i->second.dist.x), 
							  -cm_to_pixels(i->second.dist.y), 
							  cm_to_pixels(MEASURE_RANGE),
							  al_map_rgb(200, 100, 200));
		al_draw_filled_circle(cm_to_pixels(i->second.dist.x),
			-cm_to_pixels(i->second.dist.y),
			cm_to_pixels(LED_RADIUS),
			al_map_rgb(50, 100, 50));
	}
	al_set_target_bitmap(old);

}

Robo::~Robo() {
	al_destroy_bitmap(sensores_img);
}

void Robo::update() {
	pos.x += vel.x;
	pos.y += vel.y;
}

void Robo::draw() {
	ALLEGRO_TRANSFORM tf;
	ALLEGRO_TRANSFORM old = *al_get_current_transform();
	al_identity_transform(&tf);
	al_translate_transform(&tf, pos.x, pos.y);
	al_rotate_transform(&tf, vel.heading() - 90);
	al_use_transform(&tf);

	al_draw_bitmap(sensores_img, -center.x, -center.y, 0);

	al_use_transform(&old);
}

int main() {
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP  *image = NULL;
	/*
	std::map<std::string, Sensor> sensores;
	sensores.insert(std::pair<std::string, Sensor>("esquerda",			Sensor(PVector(-3, -2))));
	sensores.insert(std::pair<std::string, Sensor>("esquerda_centro",	Sensor(PVector(-1.5, 0))));
	sensores.insert(std::pair<std::string, Sensor>("centro",			Sensor(PVector(0, 1.5))));
	sensores.insert(std::pair<std::string, Sensor>("direita_centro",	Sensor(PVector(1.5, 0))));
	sensores.insert(std::pair<std::string, Sensor>("direita",			Sensor(PVector(3, -2))));
	*/
	std::map<std::string, Sensor> sensores;
	sensores.insert(std::pair<std::string, Sensor>("A-4", Sensor(PVector(0, 1.5))));
	sensores.insert(std::pair<std::string, Sensor>("B-1", Sensor(PVector(-3, 0))));
	sensores.insert(std::pair<std::string, Sensor>("B-3", Sensor(PVector(-1.5, 0))));
	sensores.insert(std::pair<std::string, Sensor>("B-4", Sensor(PVector(0, 0))));
	sensores.insert(std::pair<std::string, Sensor>("B-5", Sensor(PVector(1.5, 0))));
	sensores.insert(std::pair<std::string, Sensor>("B-7", Sensor(PVector(3, 0))));
	sensores.insert(std::pair<std::string, Sensor>("C-2", Sensor(PVector(-2.25, -1.5))));
	sensores.insert(std::pair<std::string, Sensor>("C-3", Sensor(PVector(-1.25, -1.5))));
	sensores.insert(std::pair<std::string, Sensor>("C-4", Sensor(PVector(0, -1.5))));
	sensores.insert(std::pair<std::string, Sensor>("C-5", Sensor(PVector(1.25, -1.5))));
	sensores.insert(std::pair<std::string, Sensor>("C-6", Sensor(PVector(2.25, -1.5))));
	sensores.insert(std::pair<std::string, Sensor>("D-2", Sensor(PVector(-2.25, -3))));
	sensores.insert(std::pair<std::string, Sensor>("D-6", Sensor(PVector(2.25, -3))));

	if (!al_init()) {
		return 0;
	}

	if (!al_init_primitives_addon()) {
		return 0;
	}

	if (!al_init_image_addon()) {
		return 0;
	}

	display = al_create_display(800, 600);

	if (!display) {
		return 0;
	}
	/*
	al_clear_to_color(al_map_rgb(100, 100, 100));
	al_flip_display();
	al_rest(1);

	float smallest_x = (sensores.begin())->second.dist.x,
		smallest_y = (sensores.begin())->second.dist.y,
		greatest_x = (sensores.begin())->second.dist.x,
		greatest_y = (sensores.begin())->second.dist.y;

	for (std::map<std::string, Sensor>::iterator i = sensores.begin(); i != sensores.end(); ++i) {
		if (i->second.dist.x < smallest_x) {
			smallest_x = i->second.dist.x;
		} else if (i->second.dist.x > greatest_x) {
			greatest_x = i->second.dist.x;
		}
		if (i->second.dist.y < smallest_y) {
			smallest_y = i->second.dist.y;
		} else if (i->second.dist.y > greatest_y) {
			greatest_y = i->second.dist.y;
		}
	}

	PVector center = PVector(-cm_to_pixels(smallest_x - LED_RADIUS), cm_to_pixels(greatest_y + LED_RADIUS));
	image = al_create_bitmap(cm_to_pixels(greatest_x - smallest_x + 2 * LED_RADIUS),
							 cm_to_pixels(greatest_y - smallest_y + 2 * LED_RADIUS));
	if (!image) {
		throw;
	}

	ALLEGRO_BITMAP *old = al_get_target_bitmap();
	al_set_target_bitmap(image);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	al_draw_rectangle(0.5, 0.5, cm_to_pixels(greatest_x - smallest_x + 2 * LED_RADIUS) - .5,
								cm_to_pixels(greatest_y - smallest_y + 2 * LED_RADIUS) - .5, 
								al_map_rgb(200, 200, 200),
								1);
	ALLEGRO_TRANSFORM tf;
	al_identity_transform(&tf);
	al_translate_transform(&tf, center.x, center.y);
	al_use_transform(&tf);
	for (std::map<std::string, Sensor>::iterator i = sensores.begin(); i != sensores.end(); ++i) {
		al_draw_filled_circle(cm_to_pixels(i->second.dist.x),
			-cm_to_pixels(i->second.dist.y),
			cm_to_pixels(LED_RADIUS),
			al_map_rgb(255, 255, 255));
		al_set_target_backbuffer(display);
		al_draw_bitmap(image, 400 - center.x, 300 - center.y, 0);
		al_set_target_bitmap(image);
		al_flip_display();
		al_rest(2);
	}
	al_set_target_bitmap(old);*/

	image = al_load_bitmap("image.png");

	if (!image) {
		al_destroy_display(display);
		return 0;
	}


	Robo robozito(sensores, 400, 500, 270);

	int i = 400;

	while (i--) {
		al_clear_to_color(al_map_rgb(100, 100, 100));
		al_draw_bitmap(image, 0, 0, 0);
		robozito.update();
		robozito.draw();
		al_flip_display();
		al_rest(0.05);
	}

	al_rest(2);

	al_destroy_display(display);
	al_destroy_bitmap(image);

	return 0;
}

