glyph_width = 6
glyph_height = 10
glyphs_per_row = 16
vertical_spacing = 2  # Tunable value for the vertical spacing between rows

ascii_value = 32
for row in range((126 - 32 + 1) // glyphs_per_row + 1):
    for col in range(glyphs_per_row):
        if ascii_value > 126:
            break
        x = col * glyph_width
        y = row * (glyph_height + vertical_spacing)
        print(f"{ascii_value} {x} {y}")
        ascii_value += 1
