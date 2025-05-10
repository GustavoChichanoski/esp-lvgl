import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

def generate_gradient_array(number: int):
    # Define the color stops from your CSS gradient
    colors = [
        (0.101, 0.247, 0.498),  # #1a3f7f
        (0.294, 0.549, 0.745),  # #4b8cbe
        (0.494, 0.694, 0.827),  # #7eb1d3
        (0.949, 0.427, 0.314),  # #f26d50
        (0.902, 0.216, 0.275),  # #e63746
    ]

    # Create a colormap with these colors
    cmap = LinearSegmentedColormap.from_list("custom_gradient", colors, N=number)

    # Generate 255 color values
    gradient = cmap(np.linspace(0, 1, number))

    # Convert to 8-bit RGB and format as C array
    c_array = "const uint32_t COLOR_GRADIENT[COLOR_GRADIENT_SIZE] = {\n"
    for i, color in enumerate(gradient):
        r = int(color[0] * number)
        g = int(color[1] * number)
        b = int(color[2] * number)
        hex_color = f"0x{r:02X}{g:02X}{b:02X}"
        c_array += f"    {hex_color},"
        if (i + 1) % 5 == 0:  # New line every 5 colors
            c_array += "\n"

    # Remove trailing comma and close array
    c_array = c_array.rstrip(",") + "\n};"

    return c_array, cmap

def display_gradient(cmap: LinearSegmentedColormap):
    gradient = np.linspace(0, 1, 256).reshape(1, -1)
    gradient = np.vstack((gradient, gradient))

    plt.figure(figsize=(8, 2))
    plt.imshow(gradient, aspect='auto', cmap=cmap)
    plt.axis('off')
    plt.show()

if __name__ == "__main__":
    NUMBER = 255
    c_array, cmap = generate_gradient_array(NUMBER)
    display_gradient(cmap)
    # Write to a header file
    with open("color_gradient.h", "w", encoding="utf-8") as f:
        f.write("#ifndef COLOR_GRADIENT_H\n")
        f.write("#define COLOR_GRADIENT_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define COLOR_GRADIENT_SIZE {NUMBER}\n\n")
        f.write(c_array)
        f.write("\n\n#endif // COLOR_GRADIENT_H\n")
