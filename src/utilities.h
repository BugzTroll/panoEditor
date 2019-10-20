#pragma once
#include "math.h"
#include <QVector3D>
#include <QMatrix4x4>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <functional>
#include <array>

QMatrix4x4 getRotationMatrixFromV(const QVector3D &axis, float angle)
{
	float a = cos(angle / 2.0);
	float b = -axis.x() * sin(angle / 2.0);
	float c = -axis.y() * sin(angle / 2.0);
	float d = -axis.z() * sin(angle / 2.0);
	float aa = a * a;
	float bb = b * b;
	float cc = c * c;
	float dd = d * d;
	float bc = b * c;
	float ad = a * d;
	float ac = a * c;
	float ab = a * b;
	float bd = b * d;
	float cd = c * d;

	return QMatrix4x4(float(aa + bb - cc - dd), float(2 * (bc + ad)), float(2 * (bd - ac)), 0,
		float(2 * (bc - ad)), float(aa + cc - bb - dd), float(2 * (cd + ab)), 0,
		float(2 * (bd + ac)), float(2 * (cd - ab)), float(aa + dd - bb - cc), 0,
		0, 0, 0, 1);
}

std::optional<std::string> loadFile(std::string fileName){
	std::ifstream f(fileName);

	if (f.is_open()) {
		std::ostringstream ss;
		ss << f.rdbuf();
		f.close();
		return std::optional<std::string>(ss.str());
	}
	return std::nullopt;
}
