#pragma once

#include <iostream>
#include "stb_image.h"

GLuint setup_texture(const char* filename)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// generate opengl texture object
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// params for how to draw texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int w, h, chan;
	// flip y-axis for correct orientation of texture
	stbi_set_flip_vertically_on_load(true);
	// gets pixels, width, length, and colour channels
	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);

	// load texture data
	if (pxls) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pxls);
	}
	else {
		fprintf(stderr, "setup_texture : failed to load map data.\n");
		return -1;
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[] pxls;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return texObject;
}

GLuint setup_mipmaps(const char* filename[], int n)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// generate opengl texture object
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// params for how to draw texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int w[16], h[16], chan[16];
	unsigned char* pxls[16];
	// flip y-axis for correct orientation of texture
	stbi_set_flip_vertically_on_load(true);
	// gets pixels, width, length, and colour channels
	for (int i = 0; i < n; i++) {
		pxls[i] = stbi_load(filename[i], &w[i], &h[i], &chan[i], 0);
		// load texture data
		if (pxls) {
			glTexImage2D(GL_TEXTURE_2D, i, GL_RGB, w[i], h[i], 0, GL_RGB, GL_UNSIGNED_BYTE, pxls[i]);
		}
		else {
			fprintf(stderr, "setup_mipmaps : failed to load map data.\n");
			return -1;
		}

		delete pxls[i];
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	return texObject;
}