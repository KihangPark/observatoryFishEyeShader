#include <iostream>
#include <string>
#include <cmath>
#include "shader.h"
#include "observatoryFishEyeShader.h"
#include "observatoryFishEyeShader_Util.h"

enum BlendMode {
	Normal = 0,
	Multiple = 1,
	Screen = 2,
	Overlay = 3,
	Softlight = 4,
	Hardlight = 5,
	Dodge = 6,
	Burn = 7,
	Darken = 8,
	Lighten = 9,
	Difference = 10,
	Exclusion = 11
};

#define Message(m, x) cout << m << x << endl;

// include for licence checking
#include "envCheck.h"

uint32_t productKeyHashValue;
string userName;
string registedProductKey;
int limitDate;

// global variable for checking license
miBoolean licenseCheckFlag = miTRUE;

// global offset target
miScalar offsetX = 0.f;
miScalar offsetY = 0.f;

#define _USE_MATH_DEFINES
#define toRadian(degree)    ((degree)*(M_PI/180.0f))
#define toDegree(radian)    ((radian)*(180.0f/PI))

using namespace std;

miBoolean checkLicense() {

	const int MAXLEN = 80;
	char newday[MAXLEN];
	time_t t = time(0);
	struct tm newtime;
	localtime_s(&newtime, &t);
	strftime(newday, MAXLEN, "%Y%m%d", &newtime);

	cout << "=========================================================" << endl;
	cout << "Current day : "<< newday << '\n';
	cout << "License expire day : "<< limitDate << '\n';
	cout << "Windows product key : ";

	string productKey = GetWindowsKey(true);

	size_t found;
	found = productKey.find_last_of("-");
	
	string productKeyResult = "";

	if (found != string::npos) {
		productKeyResult = productKey.substr(found+1);
	}

	cout << "XXXXX-XXXXX-XXXXX-" << productKeyResult << endl;

	cout << "=========================================================" << endl;
	
	uint32_t productKeyValue = SuperFastHash (productKeyResult.c_str() , productKeyResult.length());

	if (productKeyValue == productKeyHashValue) {

		cout << "License ProductKey Check Cleared" << endl;

		if (limitDate > atoi(newday)) {
			cout << "License Date Check Cleared." << endl;
		} else {
			cout << "License Date Check Failed." << endl;
			cout << "Your license was already expired." << endl;
			cout << "=========================================================" << endl;
			return miFALSE;
		}

	} else {

		cout << "Your productKey is not correct." << endl;
		cout << "Your registed productKey is " << registedProductKey << "." << endl;
		cout << "=========================================================" << endl;
		return miFALSE;
	}

#ifdef RAY312

	cout << "Your license is not allowed to use upper than the mentalray 3.11" << endl;
	cout << "=========================================================" << endl;
	return miFALSE;

#else

	cout << "Mentalray Version Check Cleared." << endl;
	cout << "=========================================================" << endl;
	return miTRUE;

#endif

}

miColor blendColor(miState* state, miColor* renderColor, miInteger blend_mode, miTag blend_texture, miBoolean below_layer, miScalar coordX, miScalar coordY) {

	miColor blendColor;
	miColor upper;
	miColor lower;

	miVector coord;
	coord.x = (coordX + 1) / 2.0f;
	coord.y = (coordY + 1) / 2.0f;
	mi_lookup_color_texture(&blendColor, state, blend_texture, &coord);

	if (below_layer == miTRUE) {
		upper.r = renderColor->r;
		upper.g = renderColor->g;
		upper.b = renderColor->b;
		lower.r = blendColor.r;
		lower.g = blendColor.g;
		lower.b = blendColor.b;
	} else {
		upper.r = blendColor.r;
		upper.g = blendColor.g;
		upper.b = blendColor.b;
		lower.r = renderColor->r;
		lower.g = renderColor->g;
		lower.b = renderColor->b;
	}
	
	switch (blend_mode) {

		case Normal:
			blendColor.r = upper.r + lower.r;
			blendColor.g = upper.g + lower.g;
			blendColor.b = upper.r + lower.r;
			break;
		case Multiple:
			blendColor.r = upper.r * lower.r;
			blendColor.g = upper.g * lower.g;
			blendColor.b = upper.r * lower.r;
			break;
		case Screen:
			blendColor.r = 1.0f - ((1.0f - lower.r) * (1.0f - upper.r));
			blendColor.g = 1.0f - ((1.0f - lower.g) * (1.0f - upper.g));
			blendColor.b = 1.0f - ((1.0f - lower.b) * (1.0f - upper.b));
			break;
		case Overlay:
			if (lower.r < 0.5f) {
				blendColor.r = 2.0f * lower.r * upper.r;
			} else {
				blendColor.r = 1.0f - 2.0f * (1.0f - lower.r) * (1.0f - upper.r);
			}
			if (lower.g < 0.5f) {
				blendColor.g = 2.0f * lower.g * upper.g;
			} else {
				blendColor.g = 1.0f - 2.0f * (1.0f - lower.g) * (1.0f - upper.g);
			}
			if (lower.b < 0.5f) {
				blendColor.b = 2.0f * lower.b * upper.b;
			} else {
				blendColor.b = 1.0f - 2.0f * (1.0f - lower.b) * (1.0f - upper.b);
			}
			break;
		case Softlight:
			if (upper.r < 0.5f) {
				blendColor.r = powf(lower.r, (2.0f * (1.0f - upper.r)));
			} else {
				blendColor.r = powf(lower.r, (1.0f / (2.0f * upper.r)));
			}
			if (upper.g < 0.5f) {
				blendColor.g = powf(lower.g, (2.0f * (1.0f - upper.g)));
			} else {
				blendColor.g = powf(lower.g, (1.0f / (2.0f * upper.g)));
			}
			if (upper.b < 0.5f) {
				blendColor.b = powf(lower.b, (2.0f * (1.0f - upper.b)));
			} else {
				blendColor.b = powf(lower.b, (1.0f / (2.0f * upper.b)));
			}
			break;
		case Hardlight:
			if (upper.r < 0.5f) {
				blendColor.r = lower.r * upper.r * 2.0f;
			} else {
				blendColor.r = 1.0f - 2.0f * (1.0f - lower.r) * (1.0f - upper.r);
			}
			if (upper.g < 0.5f) {
				blendColor.g = lower.g * upper.g * 2.0f;
			} else {
				blendColor.g = 1.0f - 2.0f * (1.0f - lower.g) * (1.0f - upper.g);
			}
			if (upper.b < 0.5f) {
				blendColor.b = lower.b * upper.b * 2.0f;
			} else {
				blendColor.b = 1.0f - 2.0f * (1.0f - lower.b) * (1.0f - upper.b);
			}
			break;
		case Dodge:
			blendColor.r = lower.r / (1.0f - upper.r);
			blendColor.g = lower.g / (1.0f - upper.g);
			blendColor.b = lower.b / (1.0f - upper.b);
			break;
		case Burn:
			blendColor.r = 1.0f - (1.0f - lower.r) / upper.r;
			blendColor.g = 1.0f - (1.0f - lower.g) / upper.g;
			blendColor.b = 1.0f - (1.0f - lower.b) / upper.b;
			break;
		case Darken:
			if (lower.r >= upper.r) {
				blendColor.r = upper.r;
			} else {
				blendColor.r = lower.r;
			}
			if (lower.g >= upper.g) {
				blendColor.g = upper.g;
			} else {
				blendColor.g = lower.g;
			}
			if (lower.b >= upper.b) {
				blendColor.b = upper.b;
			} else {
				blendColor.b = lower.b;
			}
			break;
		case Lighten:
			if (lower.r >= upper.r) {
				blendColor.r = lower.r;
			} else {
				blendColor.r = upper.r;
			}
			if (lower.g >= upper.g) {
				blendColor.g = lower.g;
			} else {
				blendColor.g = upper.g;
			}
			if (lower.b >= upper.b) {
				blendColor.b = lower.b;
			} else {
				blendColor.b = upper.b;
			}
			break;
		case Difference:
			blendColor.r = fabs(lower.r - upper.r);
			blendColor.g = fabs(lower.g - upper.g);
			blendColor.b = fabs(lower.b - upper.b);
			break;
		case Exclusion:
			blendColor.r = (1.0f - upper.r) * lower.r + (1.0f - lower.r) * upper.r;
			blendColor.g = (1.0f - upper.g) * lower.g + (1.0f - lower.g) * upper.g;
			blendColor.b = (1.0f - upper.b) * lower.b + (1.0f - lower.b) * upper.b;
			break;
		default:
			blendColor.r = renderColor->r;
			blendColor.g = renderColor->g;
			blendColor.b = renderColor->b;
			break;
	}
	return blendColor;
}	

void observatoryFishEyeShader_init(miState* state, struct observatoryFishEyeShader *paras, miBoolean *inst_req) {

	if (!paras) {

        *inst_req = miTRUE;

		cout << "---------------------------------------------------------" << endl;
        cout << "    observatoryFishEyeShader, version 0.9 " << endl;
		cout << "    Copyright (c) 2011-2013, Kihang Park " << endl;
		cout << "    This shader is compiled for ";
 		cout << userName << "." << endl;
        cout << "---------------------------------------------------------" << endl;

		miBoolean licenseCheck = checkLicense();
		if (licenseCheck != miTRUE) {
			licenseCheckFlag = miFALSE;
			return;
		}

	} else {

		miBoolean offset = *mi_eval_boolean(&paras->offset);
		
		miScalar offset_degree = *mi_eval_scalar(&paras->offset_degree);
		miScalar offset_radian = toRadian(offset_degree);
		miScalar offset_distance = *mi_eval_scalar(&paras->offset_distance);

		miInteger wide_degree = *mi_eval_integer(&paras->wide_degree);
		miScalar wideRadianAngle = toRadian(wide_degree);

		if (offset) {

			miVector offset_vector;
			offset_vector.x = cos(offset_radian) * offset_distance;
			offset_vector.y = sin(offset_radian) * offset_distance;
			offset_vector.z = 0;

			miVector xAxis;
			xAxis.x = 1;
			xAxis.y = 0;
			xAxis.z = 0;

			miVector yAxis;
			yAxis.x = 0;
			yAxis.y = 1;
			yAxis.z = 0;

			miVector offset_vector_normalize = offset_vector;
			mi_vector_normalize(&offset_vector_normalize);

			miScalar pi = acos(mi_vector_dot(&offset_vector_normalize, &xAxis));
			if (mi_vector_dot(&offset_vector_normalize, &yAxis) < 0.f) {
				pi = 2 * M_PI - pi;
			}

			miScalar radius = sqrt(powf(offset_vector.x, 2.0f) + powf(offset_vector.y, 2.0f));
			miScalar theta = (wideRadianAngle / 2.0) * (radius / 1.0);

			miVector offsetDir;
			if (theta > (M_PI / 2.0)) {

				theta = M_PI - theta;
				offsetDir.x = sin(theta)* cos(pi);
				offsetDir.y = sin(theta)* sin(pi);
				offsetDir.z = cos(theta);
			} else {
				offsetDir.x = sin(theta)* cos(pi);
				offsetDir.y = sin(theta)* sin(pi);
				offsetDir.z = -cos(theta);
			}

			mi_vector_normalize(&offsetDir);
			offsetX = offsetDir.x;
			offsetY = offsetDir.y;
		}
	}
}

miBoolean observatoryFishEyeShader (
    miColor *result, miState *state, struct observatoryFishEyeShader *params  )
{

	// check license
	if (licenseCheckFlag == miFALSE) return miTRUE;

	miInteger upside_degree = *mi_eval_integer(&params->upside_degree);
	miInteger wide_degree = *mi_eval_integer(&params->wide_degree);
	miScalar upsideRadianAngle = toRadian(upside_degree);
	miScalar wideRadianAngle = toRadian(wide_degree);
	miBoolean circle_type = *mi_eval_boolean(&params->circle_type);
	miBoolean guide_line = *mi_eval_boolean(&params->guide_line);

	miBoolean env_override = *mi_eval_boolean(&params->env_override);
	miInteger env_override_distance = *mi_eval_integer(&params->env_override_distance);
	miBoolean reflection_override = *mi_eval_boolean(&params->reflection_override);
	miInteger reflection_override_distance = *mi_eval_integer(&params->reflection_override_distance);
	miBoolean refraction_override = *mi_eval_boolean(&params->refraction_override);
	miInteger refraction_override_distance = *mi_eval_integer(&params->refraction_override_distance);

	miBoolean offset = *mi_eval_boolean(&params->offset);
	miScalar offset_degree = *mi_eval_scalar(&params->offset_degree);
	miScalar offset_radian = toRadian(offset_degree);
	miScalar offset_distance = *mi_eval_scalar(&params->offset_distance);

	miBoolean mask = *mi_eval_boolean(&params->mask);
	miTag mask_texture = *mi_eval_tag(&params->mask_texture);

	miBoolean background = *mi_eval_boolean(&params->background);
	miTag background_texture = *mi_eval_tag(&params->background_texture);

	miBoolean raynoise = *mi_eval_boolean(&params->raynoise);
	miColor raynoise_vector = *mi_eval_color(&params->raynoise_vector);

	miBoolean projection = *mi_eval_boolean(&params->projection);
	miScalar projection_left = *mi_eval_scalar(&params->projection_left);
	miScalar projection_right = *mi_eval_scalar(&params->projection_right);
	miScalar projection_up = *mi_eval_scalar(&params->projection_up);
	miScalar projection_down = *mi_eval_scalar(&params->projection_down);
	miTag projection_texture = *mi_eval_tag(&params->projection_texture);
	miBoolean projection_alpha = *mi_eval_boolean(&params->projection_alpha);

	miBoolean debug = *mi_eval_boolean(&params->debug);
	miBoolean ray_limit = *mi_eval_boolean(&params->ray_limit);
	miScalar ray_limit_degree = *mi_eval_scalar(&params->ray_limit_degree);

    miVector        camdir, dir;
    miScalar        x, y, r, t;

    mi_vector_to_camera(state, &camdir, &state->dir);
    t = state->camera->focal / -camdir.z / (state->camera->aperture/2);

    x = t * camdir.x;
    y = t * camdir.y * state->camera->aspect;

    if (mask == miTRUE) {
        miVector coord;
        coord.x = (x + 1) / 2.0f;
        coord.y = (y + 1) / 2.0f;
        miColor maskColor;
        mi_lookup_color_texture(&maskColor, state, mask_texture, &coord);
        if (maskColor.r == 0 && maskColor.g == 0 && maskColor.b ==0) {
            if (background == miTRUE) {
                miColor backgroundColor;
                mi_lookup_color_texture(&backgroundColor, state, background_texture, &coord);
                *result = backgroundColor;
            } else {
                *result = *mi_eval_color(&params->outside_color);
            }
            return miTRUE;
        }
    }

    r = x * x + y * y;

    if ((r < 1) || (!(circle_type))) {

        miVector xAxis;
        xAxis.x = 1;
        xAxis.y = 0;
        xAxis.z = 0;

        miVector yAxis;
        yAxis.x = 0;
        yAxis.y = 1;
        yAxis.z = 0;

        miVector direction;
        direction.x = x;
        direction.y = y;
        direction.z = 0;
        mi_vector_normalize(&direction);

        miScalar pi = acos(mi_vector_dot(&direction, &xAxis));
        if (mi_vector_dot(&direction, &yAxis) < 0.f) {
            pi = 2 * M_PI - pi;
        }

        miScalar radius = sqrt(r);
        miScalar theta = (M_PI / 2.0) * (radius / 1.0);
        miScalar theta = (wideRadianAngle / 2.0) * (radius / 1.0);

        if (projection == miTRUE) {
            miScalar projectionLeftRadianAngle = toRadian(projection_left + 180.f);
            miScalar projectionRightRadianAngle = toRadian(projection_right + 180.f);
            miScalar projectionUpRadianAngle = toRadian(projection_up);
            miScalar projectionDownRadianAngle = toRadian(projection_down);

            if (((projectionLeftRadianAngle < pi) && (pi < projectionRightRadianAngle)) &&
                ((projectionUpRadianAngle < theta) && (theta < projectionDownRadianAngle))) {
                miVector coord;
                coord.x = ((pi - projectionLeftRadianAngle) / (projectionRightRadianAngle - projectionLeftRadianAngle));
                coord.y = 1.0f - ((theta - projectionUpRadianAngle) / (projectionDownRadianAngle - projectionUpRadianAngle));
                miColor projectionColor;
                mi_lookup_color_texture(&projectionColor, state, projection_texture, &coord);
                if (projection_alpha == miTRUE) {
                    if (projectionColor.a != 0) {
                        *result = projectionColor;
                        return miTRUE;
                    }
                } else {
                    if ((projectionColor.r != 0) || (projectionColor.g != 0) || (projectionColor.b != 0)) {
                        *result = projectionColor;
                        return miTRUE;
                    }
                }
            }
        }

        miVector newDir;
        if (theta > (M_PI / 2.0)) {
            theta = M_PI - theta;
            newDir.x = sin(theta)* cos(pi);
            newDir.y = sin(theta)* sin(pi);
            newDir.z = cos(theta);
        } else {
            newDir.x = sin(theta)* cos(pi);
            newDir.y = sin(theta)* sin(pi);
            newDir.z = -cos(theta);
        }

        mi_vector_normalize(&newDir);

        if (offset) {

            newDir.x = newDir.x - offsetX;
            newDir.y = newDir.y - offsetY;

            mi_vector_normalize(&newDir);
        }

        if (raynoise == miTRUE) {
            miVector raynoiseVector;
            raynoiseVector.x = (raynoise_vector.r - 0.5f) * 2.0f;
            raynoiseVector.y = (raynoise_vector.g - 0.5f) * 2.0f;
            raynoiseVector.z = (raynoise_vector.b - 0.5f) * 2.0f;
            mi_vector_normalize(&raynoiseVector);
            newDir.x += raynoiseVector.x;
            newDir.y += raynoiseVector.y;
            newDir.z += raynoiseVector.z;
            mi_vector_normalize(&newDir);
        }

        if (guide_line) {

            miScalar unitDeg = (M_PI / 18.0f);

            int divideCount = int(theta / unitDeg);
            if ((fabs(theta - (divideCount * unitDeg)) < 0.01) || (fabs(theta - (M_PI / 2)) < 0.01)) {
                *result = *mi_eval_color(&params->guide_color);
                return miTRUE;
            }

            divideCount = int(pi / unitDeg);
            if (pi - (divideCount * unitDeg) < 0.01) {
                *result = *mi_eval_color(&params->guide_color);
                return miTRUE;
            }
        }

        miMatrix rotateMat;
        mi_matrix_rotate(rotateMat, upsideRadianAngle, 0, 0);
        mi_vector_transform(&newDir, &newDir, rotateMat);

        if (ray_limit == miTRUE) {
            if ((newDir.z > -1.0f * ray_limit_degree) && (newDir.z <= 0)){
                newDir.z = -1.0f * offset_distance;
                mi_vector_normalize(&newDir);
            }
        }

        mi_vector_from_camera(state, &dir, &newDir);

        miScalar rayDistance = 0;
		if (reflection_override || refraction_override || env_override) {

            // probe test
            // check the distance
            if (mi_trace_probe(state, &dir, &state->org)) {
                rayDistance = state->child->dist;
            }

            if (reflection_override) {
                if (reflection_override_distance < rayDistance) {
                    // set state reflection level
                    //state->reflection_level = state->options->reflection_depth - reflection_override_level;
                    state->reflection_level = state->options->reflection_depth;
                }
            }

            if (refraction_override) {
                if (refraction_override_distance < rayDistance) {
                    // set state refraction level
                    //state->refraction_level = state->options->refraction_depth - refraction_override_level;
                    state->refraction_level = state->options->refraction_depth;
                }
            }

            if (env_override) {

                if (env_override_distance < rayDistance) {
                    // trace env
                    return mi_trace_environment(result, state, &dir);
                } else {
                    // trace eye
                    return mi_trace_eye(result, state, &state->org, &dir);
                }
            }
        }

        return (mi_trace_eye(result, state, &state->org, &dir));

    } else {

        *result = *mi_eval_color(&params->outside_color);
        return miTRUE;

    }

	return miFALSE;
}

miInteger observatoryFishEyeShader_version(void) {
    return 1;
}
	    
void observatoryFishEyeShader_exit(miState* state, struct observatoryFishEyeShader* paras) {
}
