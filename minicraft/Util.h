#pragma once

float static lerp(float a, float b, float alpha) {
	float alphaSmooth = alpha * (3 * alpha - 2 * alpha*alpha);
	return (1 - alphaSmooth)*a + alphaSmooth * b;
}