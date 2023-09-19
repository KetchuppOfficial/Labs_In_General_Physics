import numpy as np

N_background = 1820

N_al = np.array([387597, 287608, 192787, 141892, 95683, 67870, 45874, 33481, 23257, 16317])
l_al = 1.98
L_al = np.arange(l_al, (N_al.size + 1) * l_al, l_al)
log_al = np.log(N_al - N_background)
coeffs_al, V_al = np.polyfit(L_al, log_al, deg=1, cov=True)
print ("Al: {} +/- {}".format(coeffs_al[0], np.sqrt(V_al[0][0])))

N_pb = np.array([396703, 254763, 177644, 132110, 102483, 84341])
l_pb = 0.42
n_pb = 6
L_pb = np.arange(l_pb, (N_pb.size + 1) * l_pb, l_pb)
log_pb = np.log(N_pb - N_background)
coeffs_pb, V_pb = np.polyfit(L_pb, log_pb, deg=1, cov=True)
print ("Fe: {} +/- {}".format(coeffs_pb[0], np.sqrt(V_pb[0][0])))

N_fe = np.array([365055, 250826, 166156, 114470, 77888, 53637, 39070, 28473, 22190, 16564, 14694, 10396])
l_fe = 1.01
L_fe = np.arange(l_fe, (N_fe.size + 1) * l_fe, l_fe)
log_fe = np.log(N_fe - N_background)
coeffs_fe, V_fe = np.polyfit(L_fe, log_fe, deg=1, cov=True)
print ("Pb: {} +/- {}".format(coeffs_fe[0], np.sqrt(V_fe[0][0])))

from matplotlib import pyplot as plt

plt.figure (figsize = (16, 9), dpi = 80)

plt.xlabel('l, см', fontsize = 24)
plt.xticks (fontsize = 20, ha = 'center', va = 'top')

plt.ylabel('$\\ln{(N - N_{ф})}$', fontsize = 24)
plt.yticks (fontsize = 20, rotation = 30, ha = 'right', va = 'top')

plt.plot (L_al, coeffs_al[0] * L_al + coeffs_al[1], color='blue', linewidth = 1)
plt.scatter (L_al, log_al, s = 15, label = 'Алюминий', color = 'blue')

plt.plot (L_fe, coeffs_fe[0] * L_fe + coeffs_fe[1], color='red', linewidth = 1)
plt.scatter (L_fe, log_fe, s = 15, label = 'Железо', color = 'red')

plt.plot (L_pb, coeffs_pb[0] * L_pb + coeffs_pb[1], color='black', linewidth = 1)
plt.scatter (L_pb, log_pb, s = 15, label = 'Свинец', color = 'black')

plt.grid (color     = 'black',
          linewidth = 0.45,
          linestyle = 'dotted')

plt.minorticks_on()

plt.grid (which     = 'minor',
          color     = 'grey',
          linewidth = 0.25,
          linestyle = 'dashed')

plt.legend (loc = 'best', fontsize = 20)

plt.savefig('./images/chart-1.png')
