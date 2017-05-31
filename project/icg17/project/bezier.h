#pragma once
#include "icg_helper.h"

class Bezier {

private:
    vector<vec3> list;
    int points = 0;

    static float bernstein(double t, int n, int i) {
        return (float) (binomial(n, i) * pow(t, i) * pow(1 - t, n - i));
    }

    static float binomial(int n, int k) {
        return fact(n) / (fact(n - k) * fact(k));
    }

    static float fact(int n) {
        if(n == 0 || n == 1) {
            return 1;
        } else {
            return fact(n - 1) * n;
        }
    }

    vec3 bezier(double t, int begin, int end) {
        vec3 Bn = vec3(0.0f);
        for(int i = 0; i < end - begin; i++) {
            vec3 Bi = list.at(begin + i) * bernstein(t, end - begin, i);
            Bn = Bn + Bi;
        }
        return Bn;
    }

public:

    vec3 bezier_point(double t) {
                return bezier(t, 0, points - 1);
        }

        void addPoint(vec3 point) {
            list.push_back(point);
            points++;
        }

        void clean() {
            list.clear();
            points = 0;
        }

        int get_points() const { return points; }
};
