# ------------------------------------------------------ #
# Do not change this file. It was produced automatically #
# ------------------------------------------------------ #

from matplotlib import pyplot as plt

from matplotlib import style
style.use('ggplot')
x = [3.410, 3.300, 3.190, 3.096, 3.000]
y = [1.180, 1.110, 1.030, 0.885, 0.960]

y_err = [0.08, 0.09, 0.08, 0.07, 0.09]

plt.scatter(x, y, s = 15, label = 'Эксперименатальные точки', color = 'green')
plt.errorbar(x, y, yerr = y_err, xerr = None, ls = 'none', color = 'blue')

line_x = [3.000, 3.410]
line_y = [0.903, 1.171]

plt.plot(line_x, line_y, linewidth = 1, color = 'red', label='Аппроксимирующая прямая')

plt.legend()
plt.title('k(1/T)')
plt.xlabel('1/T, 10^3/K')
plt.ylabel('k, К/атм')

plt.savefig('../../../Общая физика/Семестр 2/Лабы/2.1.6. Эффект Джоуля-Томсона/Van_der_Waals_2.png')
