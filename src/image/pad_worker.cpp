#include "image.h"

PadWorker::PadWorker(
    size_t left,
    size_t top,
    size_t right,
    size_t bottom,
    unsigned char r,
    unsigned char g,
    unsigned char b,
    CImg<unsigned char> * cimg,
    NanCallback * callback
): NanAsyncWorker(callback), _left(left), _top(top), _right(right),
    _bottom(bottom), _r(r), _g(g), _b(b), _cimg(cimg) {}

PadWorker::~PadWorker() {}

void PadWorker::Execute () {
    CImg<unsigned char> * res;
    size_t oldwidth = _cimg->width(),
           oldheight = _cimg->height(),
           newwidth = oldwidth + _left + _right,
           newheight = oldheight + _top + _bottom;
    if (oldwidth != newwidth || oldheight != newheight) {
        try {
            res = new CImg<unsigned char>(newwidth, newheight, 1, 3);
        } catch (CImgException e) {
            SetErrorMessage("Out of memory");
            return;
        }
        // fill left border:
        for (size_t x = 0; x < _left; x++) {
            for (size_t y = 0; y < newheight; y++) {
                res->fillC(x, y, 0, _r, _g, _b);
            }
        }
        // fill right border:
        for (size_t x = oldwidth + _left; x < newwidth; x++) {
            for (size_t y = 0; y < newheight; y++) {
                res->fillC(x, y, 0, _r, _g, _b);
            }
        }
        // fill top border:
        for (size_t x = _left; x < oldwidth + _left; x++) {
            for (size_t y = 0; y < _top; y++) {
                res->fillC(x, y, 0, _r, _g, _b);
            }
        }
        // fill bottom border:
        for (size_t x = _left; x < oldwidth + _left; x++) {
            for (size_t y = _top + oldheight; y < newheight; y++) {
                res->fillC(x, y, 0, _r, _g, _b);
            }
        }
        // paste original image:
        cimg_forXY(*_cimg, x, y) {
            unsigned char r = (*_cimg)(x, y, 0, 0),
                          g = (*_cimg)(x, y, 0, 1),
                          b = (*_cimg)(x, y, 0, 2);
            res->fillC(x + _left, y + _top, 0, r, g, b);
        }
        res->move_to(*_cimg);
        delete res;
    }
    return;
}

void PadWorker::HandleOKCallback () {
    NanScope();
    Local<Value> argv[] = {
        NanNull()
    };
    callback->Call(1, argv);
}