#include "Deck.h"
#include "casteljau.h"

#include <iostream>

Deck::Deck(unsigned int vao, unsigned int vbo, int evals) {
	card = new Card(vao, vbo);
	if (!card->load("./objs/Ace/Untitled.obj", "./objs/Ace"))
		std::cout << "failed to load" << std::endl;

	num_evals = evals;

	// card flip curve
	ctrlFlip.push_back(point(0.f, 0.f, 0.f));
	ctrlFlip.push_back(point(2.f, 9.75f, 0.f));
	ctrlFlip.push_back(point(-2.f, 9.75f, 0.f));
	ctrlFlip.push_back(point(0.f, 0.f, 0.f));
	stepFlip1 = 0;
	flipped1 = false;

	stepFlip2 = 0;
	flipped2 = false;

	stepFlip3 = 0;
	flipped3 = false;

	stepFlip4 = 0;
	flipped4 = false;
	
	stepFlip5 = 0;
	flipped5 = false;

	// first hand curves
	ctrl1.push_back(point(0.f, 7.75f, -5.6f));
	ctrl1.push_back(point(3.88f, 7.75f, 10.8));
	ctrl1.push_back(point(-9.8f, 7.75f, -2.82f));
	ctrl1.push_back(point(5.6f, 7.75f, 0.f));
	curve1 = EvaluateBezierCurve(ctrl1, num_evals);
	step1 = 0;

	ctrl12.push_back(point(0.f, 7.75f, -5.6f));
	ctrl12.push_back(point(3.88f, 7.75f, 10.8));
	ctrl12.push_back(point(-9.8f, 7.75f, -2.82f));
	ctrl12.push_back(point(5.6f, 7.78f, -0.5f));
	curve12 = EvaluateBezierCurve(ctrl12, num_evals);
	step12 = 0;

	// second hand curves
	ctrl2.push_back(point(0.f, 7.75f, -5.6f));
	ctrl2.push_back(point(-4.25f, 7.75f, 8.23f));
	ctrl2.push_back(point(8.66f, 7.75f, -4.43f));
	ctrl2.push_back(point(-5.6f, 7.75f, 0.f));
	curve2 = EvaluateBezierCurve(ctrl2, num_evals);
	step2 = 0;

	ctrl22.push_back(point(0.f, 7.75f, -5.6f));
	ctrl22.push_back(point(-4.25f, 7.75f, 8.23f));
	ctrl22.push_back(point(8.66f, 7.75f, -4.43f));
	ctrl22.push_back(point(-5.6f, 7.78f, 0.5f));
	curve22 = EvaluateBezierCurve(ctrl22, num_evals);
	step22 = 0;

	// third hand curves
	ctrl3.push_back(point(0.f, 7.75f, -5.6f));
	ctrl3.push_back(point(6.62f, 7.75f, -1.14f));
	ctrl3.push_back(point(-6.23f, 7.75f, 1.1f));
	ctrl3.push_back(point(0.f, 7.75f, 5.6f));
	curve3 = EvaluateBezierCurve(ctrl3, num_evals);
	step3 = 0;

	ctrl32.push_back(point(0.f, 7.75f, -5.6f));
	ctrl32.push_back(point(6.62f, 7.75f, -1.14f));
	ctrl32.push_back(point(-6.23f, 7.75f, 1.1f));
	ctrl32.push_back(point(0.5f, 7.78f, 5.6f));
	curve32 = EvaluateBezierCurve(ctrl32, num_evals);
	step32 = 0;

	// burn curve
	ctrlBurn.push_back(point(0.f, 7.75f, -5.6f));
	ctrlBurn.push_back(point(2.8f, 7.75f, -2.8f));
	burnCurve = EvaluateBezierCurve(ctrlBurn, num_evals);
	stepBurn1 = 0;
	stepBurn2 = 0;
	stepBurn3 = 0;

	// flop curves
	ctrlFlop1.push_back(point(0.f, 7.75f, -5.6f));
	ctrlFlop1.push_back(point(2.2f, 7.75f, -2.23f));
	ctrlFlop1.push_back(point(2.3f, 7.75f, 0.f));
	flopCurve1 = EvaluateBezierCurve(ctrlFlop1, num_evals);
	stepFlop1 = 0;

	ctrlFlop2.push_back(point(0.f, 7.75f, -5.6f));
	ctrlFlop2.push_back(point(2.2f, 7.75f, -2.23f));
	ctrlFlop2.push_back(point(1.3f, 7.75f, 0.f));
	flopCurve2 = EvaluateBezierCurve(ctrlFlop2, num_evals);
	stepFlop2 = 0;

	ctrlFlop3.push_back(point(0.f, 7.75f, -5.6f));
	ctrlFlop3.push_back(point(2.2f, 7.75f, -2.23f));
	ctrlFlop3.push_back(point(0.3f, 7.75f, 0.f));
	flopCurve3 = EvaluateBezierCurve(ctrlFlop3, num_evals);
	stepFlop3 = 0;

	// turn curve
	ctrlTurn.push_back(point(0.f, 7.75f, -5.6f));
	ctrlTurn.push_back(point(0.2f, 7.75f, -2.23f));
	ctrlTurn.push_back(point(-1.f, 7.75f, 0.f));
	turnCurve = EvaluateBezierCurve(ctrlTurn, num_evals);
	stepTurn = 0;

	// river curve
	ctrlRiver.push_back(point(0.f, 7.75f, -5.6f));
	ctrlRiver.push_back(point(-1.5f, 7.75f, -2.23f));
	ctrlRiver.push_back(point(-2.3f, 7.75f, 0.f));
	riverCurve = EvaluateBezierCurve(ctrlRiver, num_evals);
	stepRiver = 0;
}

Deck::~Deck() {
	delete card;
}

void Deck::draw(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront, const glm::vec3& camUp) {
	int deck_size = 52;
	float base_y = 7.75f;
	glm::vec3 cardPos(0.f, base_y, -5.6f);

	for (int i = 0; i < deck_size; i++) {
		card->draw(shader, camPos, camFront, camUp, cardPos, 0.f);
		cardPos.y = cardPos.y + 0.005f;
	}
}

void Deck::deal(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront, const glm::vec3& camUp, bool step) {
	// first card of first hand
	point p = curve1[step1];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step) {
		if(step1 != num_evals)
			step1++;
	}

	// first card of second hand
	p = curve2[step2];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step1 > num_evals * 0.8) {
		if (step2 != num_evals)
			step2++;
	}

	// first card of third hand
	p = curve3[step3];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step2 > num_evals * 0.8) {
		if (step3 != num_evals)
			step3++;
	}

	// second card of first hand
	p = curve12[step12];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step3 > num_evals * 0.8) {
		if (step12 != num_evals)
			step12++;
	}

	// second card of second hand
	p = curve22[step22];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step12 > num_evals * 0.8) {
		if (step22 != num_evals)
			step22++;
	}

	// second card of third hand
	p = curve32[step32];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step22 > num_evals * 0.8) {
		if (step32 != num_evals)
			step32++;
	}

	// first burn card
	p = burnCurve[stepBurn1];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && step32 > num_evals * 0.8) {
		if (stepBurn1 != num_evals)
			stepBurn1++;
	}

	// the flop
	p = flopCurve1[stepFlop1];
	if(!flipped1)
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepBurn1 > num_evals * 0.8) {
		if (stepFlop1 != num_evals)
			stepFlop1++;
	}

	p = flopCurve2[stepFlop2];
	if (!flipped2)
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepFlop1 > num_evals * 0.8) {
		if (stepFlop2 != num_evals)
			stepFlop2++;
	}

	p = flopCurve3[stepFlop3];
	if (!flipped3)
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepFlop2 > num_evals * 0.8) {
		if (stepFlop3 != num_evals)
			stepFlop3++;
	}

	// flip flop cards
	if(step && stepFlop3 > num_evals * 0.8)
		flipCard(shader, camPos, camFront, camUp, *(flopCurve1.end()-1), stepFlip1, flipped1);
	if(step && stepFlip1 == 180)
		flipCard(shader, camPos, camFront, camUp, *(flopCurve2.end() - 1), stepFlip2, flipped2);
	if (step && stepFlip2 == 180)
		flipCard(shader, camPos, camFront, camUp, *(flopCurve3.end() - 1), stepFlip3, flipped3);

	// second burn card
	p = burnCurve[stepBurn2];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepFlip3 == 180) {
		if (stepBurn2 != num_evals)
			stepBurn2++;
	}

	// turn card
	p = turnCurve[stepTurn];
	if(!flipped4)
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepBurn2 > num_evals * 0.8) {
		if (stepTurn != num_evals)
			stepTurn++;
	}

	if(step && stepTurn == num_evals)
		flipCard(shader, camPos, camFront, camUp, *(turnCurve.end() - 1), stepFlip4, flipped4);

	// third burn card
	p = burnCurve[stepBurn3];
	card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepFlip4 == 180) {
		if (stepBurn3 != num_evals)
			stepBurn3++;
	}

	// river card
	p = riverCurve[stepRiver];
	if (!flipped5)
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), 0.f);
	if (step && stepBurn3 > num_evals * 0.8) {
		if (stepRiver != num_evals)
			stepRiver++;
	}

	if (step && stepRiver == num_evals)
		flipCard(shader, camPos, camFront, camUp, *(riverCurve.end() - 1), stepFlip5, flipped5);

}

void Deck::flipCard(GLuint shader, const glm::vec3& camPos, const glm::vec3& camFront,
	const glm::vec3& camUp, const point& cardPos, int& flipStep, bool& flipped) {
	flipped = true;

	if (flipStep == 0) {
		ctrlFlip[0] = cardPos;
		ctrlFlip[3] = cardPos;
		flipCurve = EvaluateBezierCurve(ctrlFlip, 180);
	}

	point p = flipCurve[flipStep];
	float rotation = static_cast<float>(flipStep);
	if (flipStep != 180) {
		card->draw(shader, camPos, camFront, camUp, glm::vec3(p.x, p.y, p.z), rotation);
		flipStep++;
	}
	else {
		card->draw(shader, camPos, camFront, camUp, glm::vec3(cardPos.x, cardPos.y, cardPos.z), rotation);
	}
}