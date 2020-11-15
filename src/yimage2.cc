#include "config.h"

#if defined CONFIG_IMLIB2 && !defined CONFIG_GDK_PIXBUF_XLIB

#include "mregex.h"
#include "yimage.h"
#include "yxapp.h"
#include "ypointer.h"
#include <stdlib.h>
#include <Imlib2.h>

#define ATH 10  /* alpha threshold */

typedef Imlib_Image Image;

class YImage2: public YImage {
public:
    YImage2(unsigned width, unsigned height, Image image, DATA32* data = nullptr):
        YImage(width, height), fImage(image), fData(data) { }
    virtual ~YImage2() {
        context();
        imlib_free_image();
    }
    virtual ref<YPixmap> renderToPixmap(unsigned depth, bool premult);
    virtual ref<YImage> scale(unsigned width, unsigned height);
    virtual void draw(Graphics& g, int dx, int dy);
    virtual void draw(Graphics& g, int x, int y,
                       unsigned w, unsigned h, int dx, int dy);
    virtual void composite(Graphics& g, int x, int y,
                            unsigned w, unsigned h, int dx, int dy);
    virtual unsigned depth() const;
    virtual bool hasAlpha() const;
    virtual bool valid() const { return fImage != nullptr; }
    virtual ref<YImage> subimage(int x, int y, unsigned w, unsigned h);
    virtual void save(upath filename);

private:
    Image fImage;
    fsmart<DATA32> fData;

    void context() const {
        imlib_context_set_image(fImage);
    }
};

bool YImage::supportsDepth(unsigned depth) {
    return depth == xapp->depth() || depth == 32;
}

bool YImage2::hasAlpha() const {
    context();
    return imlib_image_has_alpha();
}

unsigned YImage2::depth() const {
    context();
    return imlib_image_has_alpha() ? 32 : 24;
}

ref<YImage> YImage::load(upath filename) {
    Image image = imlib_load_image_immediately_without_cache(filename.string());
    if (image) {
        imlib_context_set_image(image);
        int w = imlib_image_get_width();
        int h = imlib_image_get_height();
        DATA32* data = imlib_image_get_data_for_reading_only();
        DATA32* stop = data + w * h;
        const DATA32 limit = ATH << 24;
        if (imlib_image_has_alpha()) {
            for (DATA32* p = data; p < stop; ++p) {
                if (*p < limit) {
                    *p = 0;
                }
            }
        } else {
            for (DATA32* p = data; p < stop; ++p) {
                *p |= 0xFF000000;
            }
            imlib_image_set_has_alpha(1);
        }
        return ref<YImage>(new YImage2(w, h, image));
    }

    // support themes with indirect XPM images, like OnyX:
    const int lim = 64;
    for (int k = 9; --k > 0 && inrange(int(filename.fileSize()), 5, lim); ) {
        fileptr fp(filename.fopen("r"));
        if (fp == nullptr)
            break;

        char buf[lim];
        if (fgets(buf, lim, fp) == nullptr)
            break;

        static mregex re("^[a-z][-_a-z0-9]*\\.xpm$", "i");
        auto match = re.match(buf);
        if (match.isEmpty())
            break;

        filename = filename.parent().relative(match);
        if (filename.fileSize() > lim)
            return load(filename);
    }
    return null;
}

void YImage2::save(upath filename) {
    context();
    imlib_image_set_format("png");
    imlib_save_image(filename.replaceExtension(".png").string());
}

ref<YImage> YImage2::scale(unsigned w, unsigned h) {
    if (w == width() && h == height())
        return ref<YImage>(this);

    context();
    Image image = imlib_create_cropped_scaled_image(0, 0,
                   int(width()), int(height()), int(w), int(h));
    if (image) {
        return ref<YImage>(new YImage2(w, h, image));
    }
    return null;
}

ref<YImage> YImage2::subimage(int x, int y, unsigned w, unsigned h) {
    PRECONDITION(w <= width() && unsigned(x) <= width() - w);
    PRECONDITION(h <= height() && unsigned(y) <= height() - h);

    context();
    Image image = imlib_create_cropped_image(x, y, int(w), int(h));
    if (image) {
        return ref<YImage>(new YImage2(w, h, image));
    }
    return null;
}

ref<YImage> YImage::createFromPixmap(ref<YPixmap> pixmap) {
    return createFromPixmapAndMask(pixmap->pixmap(),
                                   pixmap->mask(),
                                   pixmap->width(),
                                   pixmap->height());
}

ref<YImage> YImage::createFromPixmapAndMask(Pixmap pixmap, Pixmap mask,
                                            unsigned width, unsigned height)
{
    imlib_context_set_drawable(pixmap);
    imlib_context_set_mask(mask);
    Image image = imlib_create_image_from_drawable(mask, 0, 0,
                                   int(width), int(height), 0);
    imlib_context_set_drawable(None);
    imlib_context_set_mask(None);
    if (image) {
        return ref<YImage>(new YImage2(width, height, image));
    }
    return null;
}

ref<YImage> YImage::createFromIconProperty(long* prop_pixels,
                                           unsigned width, unsigned height)
{
    DATA32* data = (DATA32*) malloc(width * height * sizeof(DATA32));
    if (data) {
        long* p = prop_pixels;
        DATA32* stop = data + width * height;
        const DATA32 limit = ATH << 24;
        unsigned alps = 0;
        for (DATA32* d = data; d < stop; d++, p++) {
            *d = (DATA32) *p;
            alps += (*d >= limit);
        }
        if (alps * alps >= width + height) {
            for (DATA32* d = data; d < stop; d++) {
                if (*d < limit) {
                    *d = 0;
                }
            }
        } else {
            for (DATA32* d = data; d < stop; d++) {
                *d |= 0xFF000000;
            }
        }
        Image image = imlib_create_image_using_data(
                      int(width), int(height), data);
        if (image) {
            imlib_context_set_image(image);
            imlib_image_set_has_alpha(1);
            return ref<YImage>(new YImage2(width, height, image, data));
        } else {
            free(data);
        }
    }
    return null;
}

ref<YImage> YImage::createFromPixmapAndMaskScaled(Pixmap pix, Pixmap mask,
                                                  unsigned width, unsigned height,
                                                  unsigned nw, unsigned nh)
{
    ref<YImage> image = createFromPixmapAndMask(pix, mask, width, height);
    if (image != null && (nw != width || nh != height))
        image = image->scale(nw, nh);
    return image;
}

ref<YPixmap> YImage2::renderToPixmap(unsigned depth, bool premult) {
    Pixmap pixmap = None, mask = None;

    context();
    if (depth == 0) {
        depth = xapp->depth();
    }
    if (depth == 32 || depth == 24) {
        int width = int(this->width());
        int height = int(this->height());
        Visual* visual = xapp->visualForDepth(depth);
        XImage* image = XCreateImage(xapp->display(), visual, depth,
                                     ZPixmap, 0, nullptr, width, height, 8, 0);
        if (image)
            image->data = (char *) calloc(image->bytes_per_line * height, 1);
        XImage* imask = XCreateImage(xapp->display(), visual, 1,
                                     XYPixmap, 0, nullptr, width, height, 8, 0);
        if (imask)
            imask->data = (char *) calloc(imask->bytes_per_line * height, 1);

        if (image && image->data && imask && imask->data) {
            const bool alpha = imlib_image_has_alpha();
            DATA32* data = imlib_image_get_data_for_reading_only();

            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    DATA32 pixel = data[col + row * width];
                    unsigned char red = ((pixel >> 16) & 0xFF);
                    unsigned char grn = ((pixel >> 8) & 0xFF);
                    unsigned char blu = (pixel & 0xFF);
                    unsigned char alp = !alpha ? 0x00 :
                        (pixel < (ATH << 24)) ? 0x00 : ((pixel >> 24) & 0xFF);
                    if (premult) {
                        red = (red * (alp + 1)) >> 8;
                        grn = (grn * (alp + 1)) >> 8;
                        blu = (blu * (alp + 1)) >> 8;
                    }
                    XPutPixel(image, col, row,
                              (red << 16) |
                              (grn << 8) |
                              (blu << 0) |
                              (alp << 24));
                    bool bit = (alp >= ATH);
                    XPutPixel(imask, col, row, bit);
                }
            }

            pixmap = XCreatePixmap(xapp->display(), xapp->root(),
                                   width, height, depth);
            GC gc = XCreateGC(xapp->display(), pixmap, None, None);
            XPutImage(xapp->display(), pixmap, gc, image,
                      0, 0, 0, 0, width, height);
            XFreeGC(xapp->display(), gc);

            mask = XCreatePixmap(xapp->display(), xapp->root(),
                                 width, height, 1);
            gc = XCreateGC(xapp->display(), mask, None, None);
            XPutImage(xapp->display(), mask, gc, imask,
                      0, 0, 0, 0, width, height);
            XFreeGC(xapp->display(), gc);

            XDestroyImage(image);
            XDestroyImage(imask);
        }
    }
    else {
        pixmap = XCreatePixmap(xapp->display(), xapp->root(),
                               width(), height(), depth);
        if (pixmap) {
            mask = XCreatePixmap(xapp->display(), xapp->root(),
                                 width(), height(), 1);
            imlib_context_set_drawable(pixmap);
            imlib_context_set_mask(mask);
            imlib_render_image_on_drawable(0, 0);
            imlib_context_set_drawable(None);
            imlib_context_set_mask(None);
        }
    }

    if (pixmap) {
        return createPixmap(pixmap, mask, width(), height(), depth);
    } else {
        return null;
    }
}

ref<YPixmap> YImage::createPixmap(Pixmap pixmap, Pixmap mask,
                                  unsigned w, unsigned h, unsigned depth) {
    ref<YPixmap> n;

    if (pixmap)
        n.init(new YPixmap(pixmap, mask, w, h, depth, ref<YImage>(this)));
    return n;
}

void YImage2::draw(Graphics& g, int dx, int dy) {
    composite(g, 0, 0, width(), height(), dx, dy);
}

void YImage2::draw(Graphics& g, int x, int y, unsigned w, unsigned h, int dx, int dy) {
    composite(g, x, y, w, h, dx, dy);
}

void YImage2::composite(Graphics& g, int x, int y, unsigned width, unsigned height, int dx, int dy) {
    int w = int(width);
    int h = int(height);

    if (g.xorigin() > dx) {
        if (w <= g.xorigin() - dx)
            return;
        w -= g.xorigin() - dx;
        x += g.xorigin() - dx;
        dx = g.xorigin();
    }
    if (g.yorigin() > dy) {
        if (h <= g.xorigin() - dx)
            return;
        h -= g.yorigin() - dy;
        y += g.yorigin() - dy;
        dy = g.yorigin();
    }
    if (dx + w > int(g.xorigin() + g.rwidth())) {
        if (int(g.xorigin() + g.rwidth()) <= dx)
            return;
        w = g.xorigin() + g.rwidth() - dx;
    }
    if (dy + h > int(g.yorigin() + g.rheight())) {
        if (int(g.yorigin() + g.rheight()) <= dy)
            return;
        h = g.yorigin() + g.rheight() - dy;
    }
    if (w <= 0 || h <= 0)
        return;

    const int src_x = int(dx - g.xorigin());
    const int src_y = int(dy - g.yorigin());

    context();
    imlib_context_set_drawable(g.drawable());
    // if (imlib_image_has_alpha())
    imlib_context_set_blend(1);
    imlib_render_image_part_on_drawable_at_size(src_x, src_y, w, h, dx, dy, w, h);
    imlib_context_set_drawable(None);
    imlib_context_set_blend(0);
}

void image_init() {
    imlib_context_set_display(xapp->display());
    imlib_context_set_visual(xapp->visual());
    imlib_context_set_colormap(xapp->colormap());
    imlib_context_set_color(0, 0, 0, 255);
    imlib_context_set_anti_alias(1);
    imlib_context_set_mask_alpha_threshold(ATH);
}

#endif

// vim: set sw=4 ts=4 et:
