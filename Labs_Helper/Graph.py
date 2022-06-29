# ------------------------------------------------------ #
# Do not change this file. It was produced automatically #
# ------------------------------------------------------ #

from matplotlib import pyplot as plt
import numpy as np

from matplotlib import style
plt.figure (figsize = (10, 5.625), dpi = 80)

x = np.array([3.257329, 3.267974, 3.278689, 3.289474, 3.300330, 3.311258, 3.322259, 3.333333, 3.344482, 3.355705, 3.367003, 3.378378])
y = np.array([8.433377, 8.404024, 8.379769, 8.310169, 8.252707, 8.154500, 8.119399, 8.014666, 7.955776, 7.862882, 7.777374, 7.695758])

x_err = np.array([0.005305, 0.005340, 0.005375, 0.005410, 0.005446, 0.005482, 0.005519, 0.005556, 0.005593, 0.005630, 0.005668, 0.005707])
y_err = np.array([0.001522, 0.001568, 0.001606, 0.001722, 0.001824, 0.002012, 0.002084, 0.002314, 0.002454, 0.002693, 0.002934, 0.003183])

plt.title ('ln(P) ( 1/T ). Охлаждение', fontsize = 16)

plt.scatter (x, y, s = 15, label = 'Эксперименатальные точки', color = 'green')

line_x = [3.257329, 3.378378]
line_y = [8.492113, 7.726152]

plt.plot (line_x, line_y, linewidth = 1, color = 'red', label = 'Прямая y = -6.327675x + 29.103432')
plt.errorbar (x, y, xerr = x_err, yerr = y_err, color = 'blue', ls = 'none')

plt.xlabel ('1/T, 1000 * 1/К', fontsize = 16)
plt.xticks (fontsize = 16, ha = 'center', va = 'top')

plt.ylabel ('ln(P), ln(Па)', fontsize = 16)
plt.yticks (fontsize = 16, rotation = 30, ha = 'right', va = 'top')

plt.grid (color     = 'black',
          linewidth = 0.45,
          linestyle = 'dotted')

plt.minorticks_on ()

plt.grid (which     = 'minor',
          color     = 'grey',
          linewidth = 0.25,
          linestyle = 'dashed')

plt.legend (loc = 'upper right')
plt.savefig ('/mnt/d/Общая физика/Семестр 2/Лабы/2.4.1. Фазовые переходы/График 4.png')
