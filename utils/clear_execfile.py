import os
import glob

flist = glob.glob('**/*.exe', recursive=True) + glob.glob('**/*.out', recursive=True)

err_count = 0

for fn in flist:
    print(f'Removing `\033[1;34m{fn}\033[0m`', end=' ')
    try:
        os.remove(fn)
    except PermissionError:
        print('- \033[1;31mFailed: Permission Denied.\033[0m')
        err_count += 1
    else:
        print('- \033[1;32mOK\033[0m')

print(f'\nRemoved {len(flist) - err_count}, Failed {err_count}')



