""" the pygame transfrom module """

import math

from pygame._error import SDLError
from pygame._sdl import ffi, sdl
from pygame.surface import Surface
from pygame.surflock import locked


def new_surface_from_surface(c_surface, w, h):
    if 4 < c_surface.format.BytesPerPixel <= 0:
        raise ValueError("unsupported Surface bit depth for transform")

    format = c_surface.format
    newsurf = sdl.SDL_CreateRGBSurface(c_surface.flags, w, h,
                                       format.BitsPerPixel,
                                       format.Rmask, format.Gmask,
                                       format.Bmask, format.Amask)
    if not newsurf:
        SDLError.from_sdl_error()

    if format.BytesPerPixel == 1 and format.palette:
        sdl.SDL_SetColors(newsurf, format.palette.colors, 0,
                          format.palette.ncolors)

    if c_surface.flags & sdl.SDL_SRCCOLORKEY:
        sdl.SDL_SetColorKey(newsurf, (c_surface.flags & sdl.SDL_RLEACCEL) |
                            sdl.SDL_SRCCOLORKEY, format.colorkey)

    if c_surface.flags & sdl.SDL_SRCALPHA:
        result = sdl.SDL_SetAlpha(newsurf, c_surface.flags, format.alpha)
        if result == -1:
            raise SDLError.from_sdl_error()

    return newsurf


def flip(surface, xaxis, yaxis):
    c_surf = surface._c_surface
    w, h = c_surf.w, c_surf.h
    new_surf = new_surface_from_surface(c_surf, w, h)
    bpp = c_surf.format.BytesPerPixel
    pitch = c_surf.pitch

    with locked(new_surf):
        with locked(surface._c_surface):
            # only have to deal with rows
            if not xaxis:
                srcpixels = ffi.cast('uint8_t*', c_surf.pixels)
                destpixels = ffi.cast('uint8_t*', new_surf.pixels)
                if not yaxis:
                    # no changes - just copy pixels
                    destpixels[0:h * pitch] = srcpixels[0:h * pitch]
                else:
                    for y in range(h):
                        dest_start = (h - y - 1) * pitch
                        src_start = y * pitch
                        destpixels[dest_start:dest_start + pitch] = \
                                srcpixels[src_start:src_start + pitch]
            # have to calculate position for individual pixels
            else:
                if not yaxis:
                    def get_y(y):
                        return y
                else:
                    def get_y(y):
                        return h - y - 1

                if bpp in (1, 2, 4):
                    ptr_type = 'uint%s_t*' % c_surf.format.BitsPerPixel
                    srcpixels = ffi.cast(ptr_type, c_surf.pixels)
                    destpixels = ffi.cast(ptr_type, new_surf.pixels)
                    for y in range(h):
                        dest_row_start = get_y(y) * w
                        src_row_start = y * w
                        for x in range(w):
                            destpixels[dest_row_start + (w - x - 1)] = \
                                    srcpixels[src_row_start + x]
                else:
                    srcpixels = ffi.cast('uint8_t*', c_surf.pixels)
                    destpixels = ffi.cast('uint8_t*', new_surf.pixels)
                    for y in range(h):
                        dest_row_start = get_y(y) * pitch
                        src_row_start = y * pitch
                        for x in range(0, pitch, 3):
                            dest_pix_start = dest_row_start + (pitch - x - 3)
                            src_pix_start = src_row_start + x
                            destpixels[dest_pix_start:dest_pix_start + 3] = \
                                srcpixels[src_pix_start:src_pix_start + 3]

    return Surface._from_sdl_surface(new_surf)


def rotate(surface, angle):
    c_surf = surface._c_surface

    # special treatment if rotating by 90 degrees
    if abs(angle) == 90.0 or abs(angle) == 180.0 or abs(angle) == 0.0:
        numturns = (angle / 90) % 4
        if numturns < 0:
            numturns = 4 + numturns
        if numturns % 2 == 0:
            width = c_surf.w
            height = c_surf.h
        else:
            width = c_surf.h
            height = c_surf.w
        new_surf = new_surface_from_surface(c_surf, width, height)
        with locked(new_surf):
            with locked(c_surf):
                sdl.rotate90(c_surf, new_surf, int(angle))

    else:
        radangle = angle * 0.01745329251994329
        sangle = math.sin(radangle)
        cangle = math.cos(radangle)
        x, y = c_surf.w, c_surf.h
        cx, cy = cangle * x, cangle * y
        sx, sy = sangle * x, sangle * y
        nxmax = int(max(max(max(abs(cx + sy), abs(cx - sy)),
                            abs(-cx + sy)), abs(-cx - sy)))
        nymax = int(max(max(max(abs(sx + cy), abs(sx - cy)),
                            abs(-sx + cy)), abs(-sx - cy)))
        new_surf = new_surface_from_surface(c_surf, nxmax, nymax)

        if c_surf.flags & sdl.SDL_SRCCOLORKEY:
            bgcolor = c_surf.format.colorkey
        else:
            bgcolor = surface.get_at_mapped((0, 0))
            bgcolor &= ~(c_surf.format.Amask)

        with locked(new_surf):
            with locked(c_surf):
                sdl.rotate(c_surf, new_surf, bgcolor, sangle, cangle)

    return Surface._from_sdl_surface(new_surf)


def scale(surface, (width, height), dest_surface=None):
    """ scale(Surface, (width, height), DestSurface = None) -> Surface
    resize to new resolution
    """
    if width < 0 or height < 0:
        raise ValueError("Cannot scale to negative size")

    c_surf = surface._c_surface

    if dest_surface is None:
        new_surf = new_surface_from_surface(c_surf, width, height)
    else:
        new_surf = dest_surface._c_surface

    if new_surf.w != width or new_surf.h != height:
        raise ValueError("Destination surface not the given width or height.")

    if c_surf.format.BytesPerPixel != new_surf.format.BytesPerPixel:
        raise ValueError(
            "Source and destination surfaces need the same format.")

    if width and height:
        with locked(new_surf):
            with locked(c_surf):
                sdl.stretch(c_surf, new_surf)

    return Surface._from_sdl_surface(new_surf)


def rotozoom(surface, angle, scale):
    """ rotozoom(Surface, angle, scale) -> Surface
    filtered scale and rotation
    """
    raise NotImplementedError


def scale2x(surface, dest_surface=None):
    """ scale2x(Surface, DestSurface = None) -> Surface
    specialized image doubler
    """
    c_surf = surface._c_surface
    if dest_surface:
        new_surf = dest_surface._c_surface
        if (new_surf.w != 2 * c_surf.w) or (new_surf.h != 2 * c_surf.h):
            raise ValueError("Destination surface not 2x bigger")
    else:
        new_surf = new_surface_from_surface(c_surf, c_surf.w * 2, c_surf.h * 2)

    with locked(new_surf):
        with locked(c_surf):
            sdl.scale2x(c_surf, new_surf)

    if dest_surface:
        return dest_surface
    return Surface._from_sdl_surface(new_surf)


def smoothscale(surface, (width, height), dest_surface=None):
    """ smoothscale(Surface, (width, height), DestSurface = None) -> Surface
    scale a surface to an arbitrary size smoothly
    """
    raise NotImplementedError


def get_smoothscale_backend():
    """ get_smoothscale_backend() -> String
    return smoothscale filter version in use: 'GENERIC', 'MMX', or 'SSE'
    """
    raise NotImplementedError


def set_smoothscale_backend(type):
    """ set_smoothscale_backend(type) -> None
    set smoothscale filter version to one of: 'GENERIC', 'MMX', or 'SSE'
    """
    raise NotImplementedError


def chop(surface, rect):
    """ chop(Surface, rect) -> Surface
    gets a copy of an image with an interior area removed
    """
    raise NotImplementedError


def laplacian(surface, dest_surface=None):
    """ laplacian(Surface, DestSurface = None) -> Surface
    find edges in a surface
    """
    raise NotImplementedError


def average_surfaces(surface, dest_surface=None, palette_colors=1):
    """ average_surfaces(Surfaces, DestSurface = None, palette_colors = 1) -> Surface
    find the average surface from many surfaces.
    """
    raise NotImplementedError


def average_color(surface, rect=None):
    """ average_color(Surface, Rect = None) -> Color
    finds the average color of a surface
    """
    raise NotImplementedError


def threshold(dest_surface, surface, color, threshold=(0,0,0,0),
              diff_color=(0,0,0,0), change_return=1,
              threshold_surface=None, inverse=False):
    """ threshold(DestSurface, Surface, color, threshold = (0,0,0,0), diff_color = (0,0,0,0), change_return = 1, Surface = None, inverse = False) -> num_threshold_pixels
    finds which, and how many pixels in a surface are within a threshold of a color.
    """
    raise NotImplementedError
