#include <list>
#include <vector>
#include <algorithm>

#include "point.h"

point evaluate(float t, std::list<point> P)
{
	std::list<point> Q = P;
	while (Q.size() > 1) {
		std::list<point> R;
		std::list<point>::iterator p1 = Q.begin();
		std::list<point>::iterator p2 = Q.begin();
		p2++;

		while (p2 != Q.end()) {
			R.push_back(((1 - t) * (*p1)) + (t * (*p2)));
			p2++;
			p1++;
		}

		Q.clear();
		Q = R;
	}
	return *Q.begin();
}

std::vector<point> EvaluateBezierCurve(std::vector<point>ctrl_points, int num_evaluations)
{
	std::list<point> ps(ctrl_points.begin(), ctrl_points.end());
	std::vector<point> curve;

	float offset = static_cast<float>(1) / num_evaluations;
	curve.push_back(*ps.begin());

	for (int e = 0; e < num_evaluations; e++)
		curve.push_back(evaluate(offset * (e + 1), ps));

	return curve;
}

float* MakeFloatsFromVector(std::vector<point> curve, int& num_verts, int& num_floats, float r, float g, float b)
{
	num_verts = curve.size();
	if(num_verts == 0)
		return nullptr;

	num_floats = num_verts * 6;
	float* fs = (float*)malloc(sizeof(float) * num_floats);

	for (int p = 0; p < num_verts; p++) {
		*(fs + (p * 6)) = curve[p].x;
		*(fs + (p * 6) + 1) = curve[p].y;
		*(fs + (p * 6) + 2) = curve[p].z;
		*(fs + (p * 6) + 3) = r;
		*(fs + (p * 6) + 4) = g;
		*(fs + (p * 6) + 5) = b;
	}
	return fs;
}