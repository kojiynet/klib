
REM "%%" is interpreted as "%" in batch files

ffmpeg -r 30 -i pclub05out%%03d.png -vcodec libx264 -pix_fmt yuv420p -r 360 pclub05out.mp4

ffmpeg -r 30 -i pclub05out%%03d.png -vcodec mpeg4 -qscale 0 -r 360 pclub05out.mov
