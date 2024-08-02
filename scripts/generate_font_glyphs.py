glyph_width = 4
glyph_height = 6
glyphs_per_row = 16
vertical_spacing = 0  # Tunable value for the vertical spacing between rows

ascii_range_start = 0
ascii_range_end = 127 - 1
ascii_value = ascii_range_start
for row in range((ascii_range_end - ascii_range_start + 1) // glyphs_per_row + 1):
    for col in range(glyphs_per_row):
        if ascii_value > ascii_range_end:
            break
        x = col * glyph_width
        y = row * (glyph_height + vertical_spacing)
        print(f"{ascii_value} {x} {y}")
        ascii_value += 1
