#pragma once

#include "Card.h"
#include "point.h"

class Deck {
	public:
		Deck(unsigned int vao, unsigned int vbo, int evals);

		~Deck();

		void draw(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront, 
				const glm::vec3& camUp);

		bool deal(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront, 
				const glm::vec3& camUp, bool step);

		bool in(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront,
				const glm::vec3& camUp, bool step);

	private:
		void flipCard(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront, 
				const glm::vec3& camUp, const point& cardPos, int& flipStep, bool& flipped, bool step);

		void unFlipCard(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront,
				const glm::vec3& camUp, const point& cardPos, int& flipStep, bool& flipped, bool step);

		int num_evals;
		Card* card;

		std::vector<point> ctrlFlip;
		std::vector<point> flipCurve;
		int stepFlip1;
		bool flipped1;

		int stepFlip2;
		bool flipped2;

		int stepFlip3;
		bool flipped3;

		int stepFlip4;
		bool flipped4;

		int stepFlip5;
		bool flipped5;

		std::vector<point> ctrl1;
		std::vector<point> curve1;
		int step1;

		std::vector<point> ctrl12;
		std::vector<point> curve12;
		int step12;

		std::vector<point> ctrl2;
		std::vector<point> curve2;
		int step2;

		std::vector<point> ctrl22;
		std::vector<point> curve22;
		int step22;

		std::vector<point> ctrl3;
		std::vector<point> curve3;
		int step3;

		std::vector<point> ctrl32;
		std::vector<point> curve32;
		int step32;

		std::vector<point> ctrlBurn;
		std::vector<point> burnCurve;
		int stepBurn1;
		int stepBurn2;
		int stepBurn3;

		std::vector<point> ctrlFlop1;
		std::vector<point> flopCurve1;
		int stepFlop1;
		std::vector<point> ctrlFlop2;
		std::vector<point> flopCurve2;
		int stepFlop2;
		std::vector<point> ctrlFlop3;
		std::vector<point> flopCurve3;
		int stepFlop3;

		std::vector<point> ctrlTurn;
		std::vector<point> turnCurve;
		int stepTurn;

		std::vector<point> ctrlRiver;
		std::vector<point> riverCurve;
		int stepRiver;
};