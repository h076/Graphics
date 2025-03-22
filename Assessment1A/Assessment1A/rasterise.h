#pragma once

void ClearColourBuffer(float col[4])
{
	glm::vec3 colour(col[0], col[1], col[2]);
	for (int y = 0; y < PIXEL_H; y++)
		for (int x = 0; x < PIXEL_W; x++)
			writeColToDisplayBuffer(colour, x, y);
}

void ClearDepthBuffer()
{
	for (int i = 0; i < (PIXEL_W * PIXEL_H); i++)
		depth_buffer[i] = 1.f;
}

void ApplyTransformationMatrix(glm::mat4 T, vector<triangle>& tris)
{
	for (auto tri_ptr = tris.begin(); tri_ptr != tris.end(); tri_ptr++) {
		tri_ptr->v1.pos = T * tri_ptr->v1.pos;
		tri_ptr->v2.pos = T * tri_ptr->v2.pos;
		tri_ptr->v3.pos = T * tri_ptr->v3.pos;
	}
}

void ApplyPerspectiveDivision(vector<triangle>& tris)
{
	float clipW;
	for (auto tri_ptr = tris.begin(); tri_ptr != tris.end(); tri_ptr++) {
		clipW = tri_ptr->v1.pos.w;
		tri_ptr->v1.pos = tri_ptr->v1.pos / clipW;
		clipW = tri_ptr->v2.pos.w;
		tri_ptr->v2.pos = tri_ptr->v2.pos / clipW;
		clipW = tri_ptr->v3.pos.w;
		tri_ptr->v3.pos = tri_ptr->v3.pos / clipW;
	}
}

void ApplyViewportTransformation(int w, int h, vector<triangle>& tris)
{
	float halfW = static_cast<float>(w) * 0.5f;
	float halfH = static_cast<float>(h) * 0.5f;

	for (auto& tri : tris)
	{
		tri.v1.pos = glm::vec4((tri.v1.pos.x + 1.0f) * halfW,
			(tri.v1.pos.y + 1.0f) * halfH,
			(tri.v1.pos.z + 1.0f) * 0.5f,
			tri.v1.pos.w);

		tri.v2.pos = glm::vec4((tri.v2.pos.x + 1.0f) * halfW,
			(tri.v2.pos.y + 1.0f) * halfH,
			(tri.v2.pos.z + 1.0f) * 0.5f,
			tri.v2.pos.w);

		tri.v3.pos = glm::vec4((tri.v3.pos.x + 1.0f) * halfW,
			(tri.v3.pos.y + 1.0f) * halfH,
			(tri.v3.pos.z + 1.0f) * 0.5f,
			tri.v3.pos.w);
	}
}

float line(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return ((y2 - y1) * x3) + ((x1 - x2) * y3) + (x2 * y1) - (x1 * y2);
}

void ComputeBarycentricCoordinates(int px, int py, triangle t, float& alpha, float& beta, float& gamma)
{
	float ax = t.v1.pos.x;
	float ay = t.v1.pos.y;

	float bx = t.v2.pos.x;
	float by = t.v2.pos.y;

	float cx = t.v3.pos.x;
	float cy = t.v3.pos.y;

	alpha = line(bx, by, cx, cy, px, py) / line(bx, by, cx, cy, ax, ay);
	beta = line(ax, ay, cx, cy, px, py) / line(ax, ay, cx, cy, bx, by);
	gamma = line(ax, ay, bx, by, px, py) / line(ax, ay, bx, by, cx, cy);
}

void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, glm::vec3& col, float& depth)
{
	col = alpha * tri.v1.col + beta * tri.v2.col + gamma * tri.v3.col;

	depth = alpha * tri.v1.pos.z + beta * tri.v2.pos.z + gamma * tri.v3.pos.z;
	depth = std::clamp(depth, 0.0f, 1.0f);
}


void Rasterise(vector<triangle> tris)
{
	float alpha, beta, gamma;
	float depth = 1.0f;
	glm::vec3 col(0.f, 0.f, 0.f);
	for (int py = 0; py < PIXEL_H; py++)
	{
		float percf = (float)py / (float)PIXEL_H;
		int perci = percf * 100;
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		for (int px = 0; px < PIXEL_W; px++)
		{
			for (auto tri = tris.begin(); tri != tris.end(); tri++) {

				ComputeBarycentricCoordinates(px, py, *tri, alpha, beta, gamma);

				if (alpha >= 0 && beta >= 0 && gamma >= 0 && alpha <= 1 && beta <= 1 && gamma <= 1) {

					ShadeFragment(*tri, alpha, beta, gamma, col, depth);
				
					if (depth <= depth_buffer[px + py * PIXEL_W]) {

						writeColToDisplayBuffer(col, px, PIXEL_H - py);
						depth_buffer[px + py * PIXEL_W] = depth;
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void render(vector<triangle>& tris)
{
	float col[4] = {1.f, 1.f, 1.f, 0.f};
	ClearColourBuffer(col);
	ClearDepthBuffer();

	int num_tris = tris.size();
	
	// quad 
	if (num_tris == 2) {
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0, 0, -1));
		ApplyTransformationMatrix(model, tris);
	}else if (num_tris == 32) { // cornell2
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.1f, -2.5f, -6.f));
		ApplyTransformationMatrix(model, tris);
	}

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(60.f), static_cast<float>(PIXEL_W) / static_cast<float>(PIXEL_H), 0.1f, 10.f);
	ApplyTransformationMatrix(projection, tris);


	ApplyPerspectiveDivision(tris);
	ApplyViewportTransformation(PIXEL_W, PIXEL_H, tris);
	Rasterise(tris);
}
