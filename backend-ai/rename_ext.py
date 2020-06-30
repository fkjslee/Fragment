import glob
import os

files = glob.glob('./fragment*.png')
for f in files:
    name = int(f[:-4].split('_')[1])
    new_name = name + 9
    # new_name = 'fragment_%04d.png' % new_name
    if len(f.split('_')) == 3:
        new_name = 'fragment_%04d_m.png' % new_name
    elif len(f.split('_')) == 4:
        new_name = 'fragment_%04d_m_ext.png' % new_name
    os.rename(f, new_name)