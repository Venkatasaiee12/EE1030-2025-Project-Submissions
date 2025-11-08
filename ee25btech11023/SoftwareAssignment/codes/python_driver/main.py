import numpy as  np
from PIL import Image
import time

K=[5,20,50,100]

img = Image.open('../../figs/einstein/einstein.jpg').convert('L')

given = np.array(img,dtype=np.float64)

for i, k in enumerate(K):

    start_time = time.time()
    U,S,VT = np.linalg.svd(given, full_matrices=False)
    S_new = np.diag(S)
    U_k = U[:, :k]
    S_k = S_new[:k, :k]
    VT_k = VT[:k, :]
    out = U_k@(S_k@VT_k)
    end_time=time.time()
    print(f"Reconstruction time for rank {k}: {end_time-start_time:.6f} seconds")
