#pragma once
#include "icg_helper.h"

//class Bezier {

//private:
//    vector<vec3> list;
//    int points = 0;
//    int points_in_knot = 4;
//    int knots = 0;

//    static float bernstein(double t, int n, int i) {
//        return (float) (binomial(n, i) * pow(t, i) * pow(1 - t, n - i));
//    }

//    static float binomial(int n, int k) {
//        return fact(n) / (fact(n - k) * fact(k));
//    }

//    static float fact(int n) {
//        if(n == 0 || n == 1) {
//            return 1;
//        } else {
//            return fact(n - 1) * n;
//        }
//    }

//    vec3 bezier(double t, int begin, int end) {
//        vec3 Bn = vec3(0.0f);
//        for(int i = 0; i < end - begin; i++) {
//            vec3 Bi = list.at(begin + i) * bernstein(t, end - begin, i);
//            Bn = Bn + Bi;
//        }
//        return Bn;
//    }

//public:

//    vec3 bezier_point(double t) {
//            if (knots < 1) {
//                double u = t / points;
//                return bezier(u, 0, points - 1);
//            } else {
//                int begin = floor(t / (points_in_knot - 1)) * (points_in_knot - 1);
//                int end = begin + (points_in_knot - 1);

//                if (end > knots * (points_in_knot - 1)) {
//                    begin = knots * (points_in_knot - 1);
//                    end = points - 1;
//                }
//                int diff = end - begin;
//                double u = (t - begin) / diff;
//                return bezier(u, begin, end);
//            }
//        }

//        void addPoint(vec3 &point) {
//            if (points > (points_in_knot - 1) && points % (points_in_knot - 1) == 0) {
//                int knot_point = points - (points_in_knot - 1);
//                list.at(knot_point) = (list.at(knot_point - 1) + list.at(knot_point + 1)) * 0.5f;
//                knots++;
//            }
//            list.push_back(point);
//            points++;
//        }

//        void clean() {
//            list.clear();
//            knots = 0;
//            points = 0;
//        }

//        int get_points() const { return points; }



//};

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

        void addPoint(vec3 &point) {
            list.push_back(point);
            points++;
        }

        void clean() {
            list.clear();
            points = 0;
        }

        int get_points() const { return points; }
};
