import os
from struct import pack_into as f_pack_into

# gx2Enum.py from https://github.com/aboood40091/GTX-Extractor-Rewrite/blob/master/gx2Enum.py
# gx2Texture.py from https://github.com/aboood40091/GTX-Extractor-Rewrite/blob/master/gx2Texture.py
# main_export_png.py from https://github.com/aboood40091/GTX-Extractor-Rewrite/blob/master/main_export_png.py

from gx2Enum import GX2SurfaceDim, GX2SurfaceFormat, GX2TileMode, GX2CompSel
from gx2Texture import GX2Texture
from main_export_png import GX2TextureToPNG

from curl_noise_tex_data import g_curlNoiseTbl32

textures = []

for z in range(32):
    data = bytearray(32 * 32 * 4)

    for y in range(32):
        for x in range(32):
            pos = y * 32 + x
            f_pack_into(">4B", data, pos * 4, *g_curlNoiseTbl32[z * 1024 + pos])

    texture = GX2Texture.initTexture(
        GX2SurfaceDim.Dim2D, 32, 32, 1,
        0, GX2SurfaceFormat.Unorm_RGBA8, GX2CompSel.RGBA,
        GX2TileMode.Linear_Special,
    )

    imageData = bytes(data); assert len(imageData) >= texture.surface.imageSize
    texture.surface.imageData = imageData[:texture.surface.imageSize]

    textures.append(texture)

for i, texture in enumerate(textures):
    next(GX2TextureToPNG(texture, i == 0)).save('texture_%d.png' % i)
