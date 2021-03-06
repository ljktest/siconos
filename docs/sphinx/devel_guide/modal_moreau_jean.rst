.. _modal_moreau_jean:

Time integration of the dynamics - Exact scheme
===============================================

This page describes the discretisation of the dynamics for second-order (Lagrangian) systems of the form

.. math::
   :label: LTIDS

   Mdv + Kqdt + cv^+dt = dr \\
   v(t) = \dot q^+(t) \\
   q(t_0) = q_0, \ \dot q(t_0) = v_0

with usual notations in Siconos, where the matrices :math:`M, K` and :math:`C` are diagonals, and with

.. math::
   :label: displ

   q(t) = q_0 + \int_{t_0}^t v^+(t)


:math:`v^+(t)` stands for right limit of v in t

and with the vectors :math:`q = [q_k], v = [v_k], k\in[0,ndof-1]`.

The feature of the proposed scheme is to combine an exact method for the linear (non-contacting) part of the equations of motion with a Moreau-Jean time-stepping
approach to handle impulses and velocity jumps. This is the implementation of what is proposed in :cite:`Issanchou.2018` for the simulation of musical string instruments.
Some simulation examples are proposed in examples/Mechanics/Music directory of Siconos repository.


Summary
-------




Detailed calculations
---------------------


For comparison purpose, we consider two different schemes, the classical (in Siconos) 'Moreau-Jean' and an implementation of what is proposed in :cite:`Bilbao.2008`, denoted respectively MJ and BMJ in the following.


Let us integrate the dynamics over a time step, :math:`\Delta t = t^{i+1} - t^i`.

Notations :

.. math::

   q_k(t^i) = q_k^i \\
   v_k(t^{i}) = v_k^i \\
   
In the following, we will use k for space (bottom) indices and i for time (top) indices.
   
MJ is based on a theta-scheme, for :math:`\theta \in [0,1]`

.. math::
MJ we consider diagonal stiffness and damping, 

.. math::
   :nowrap:

   \begin{eqnarray}
      M = diag(\mu) \\
      K = \mu.diag(\omega_k^2) \\
      C = \mu.diag(2\sigma_k)
   \end{eqnarray}


:math:`\omega_k` and :math:`\sigma_k` being respectively the modal pulsation and the damping parameter (see for instance values taken from :cite:`Issanchou.2018`).

Bilbao exact scheme writes:

.. math::

   \begin{array}{ccc}
   Kq &\approx \Gamma Kq^i + \frac{(\mathcal{I}-\Gamma)K}{2}(q^{i+1} + q^{i-1}) \\
   C\dot q &\approx \frac{1}{\Delta t}\Sigma^*(q^{i+1} - q^{i-1}) \\
   \end{array}

for :math:`\Gamma = diag(\gamma_k)` and :math:`\Sigma^* = diag(\sigma_k^*)` some diagonal matrices, with

.. math::
   
   \gamma_{k} &= \frac{2}{\omega_k^2\Delta t^2} - \frac{A_k}{1+e_k-A_k}, \\
   \sigma^*_{k} &= \left(\frac{1}{\Delta t} + \frac{\omega_k^2\Delta t}{2} - \gamma_k\frac{\omega_k^2\Delta t}{2} \right)\frac{1-e_k}{1+e_k} \\
   A_k &= e^{-\sigma_k\Delta t}\left(e^{\sqrt{\sigma_k^2 - \omega_k^2}\Delta t} + e^{-\sqrt{\sigma_k^2 - \omega_k^2}\Delta t}\right) \\
   e_k &= e^{-2\sigma_k\Delta t} \\

.. math::

   \begin{array}{c|c|c}
   Dynamics       & Moreau-Jean                       &         Modal-Moreau-Jean \\
   \int_{t^i}^{t^{i+1}} Mdv & \approx M(v^{i+1}-v^{i}) & \approx M(v^{i+1}-v^{i}) \\
   \int_{t^i}^{t^{i+1}} Kqdt & \approx \Delta t(\theta Kq^{i+1} + (1 - \theta) Kq^i) & \approx \Delta t\Gamma Kq^i + \frac{\Delta t}{2}(\mathcal{I}-\Gamma)K(q^{i+1} + q^{i-1}) \\
   \int_{t^i}^{t^{i+1}} Cvdt & \approx \Delta t(\theta Cv^{i+1} + (1 - \theta) Cv^i) & \approx \Sigma^*(q^{i+1} - q^{i-1})\\
   \int_{t^i}^{t^{i+1}} dr & \approx p^{i+1} & \approx p^{i+1} \\
    \end{array}

For MJ, this leads to

.. math::

   M(v^{i+1}-v^{i}) + \Delta t(\theta Kq^{i+1} + (1 - \theta) Kq^i) + \Delta t(\theta Cv^{i+1} + (1 - \theta) Cv^i) &= p^{i+1} \\

using :math:`q^{i+1} = q^i + \Delta t(\theta v^{i+1} + (1 - \theta) v^i)`, we get

.. math::
   
   [M + \Delta t^2\theta^2 K + \Delta t\theta C] (v^{i+1}-v^{i}) + \Delta tKq^i + (\Delta t^2\theta K + \Delta tC) v^i = p^{i+1} \\

And for BMJ:

.. math::

   M(v^{i+1}-v^{i}) + \Delta t\Gamma Kq^i + \frac{\Delta t}{2}(\mathcal{I}-\Gamma)K(q^{i+1} + q^{i-1}) +\Sigma^*(q^{i+1} - q^{i-1}) = p^{i+1}

With :math:`q^{i+1} = q^{i} + \Delta tv^{i+1}`, we get

.. math::
   
   q^{i+1} - q^{i-1} &= \Delta t(v^{i+1} + v^i) \\
   q^{i+1} + q^{i-1} &= 2q^i + \Delta t(v^{i+1} - v^i) \\

and

.. math::
   
   [M + \frac{\Delta t^2}{2}(\mathcal{I} - \Gamma)K + \Delta t\Sigma^*] (v^{i+1}-v^{i}) + \Delta tKq^i + 2\Delta t \Sigma^* v^i = p^{i+1} \\
   

Both discretisations writes
   
.. math::
   
   W(v^{i+1}-v^{i}) = v_{free}^i + p^{i+1} \\
   or \\
   v^{i+1} = v^i_{free} + W^{-1}p^{i+1} \\

with

.. math::

   \begin{array}{c|c|c}
   & Moreau-Jean                       &         Modal-Moreau-Jean \\
   W & = M + \Delta t^2\theta^2 K + \Delta t\theta C & = M + \frac{\Delta t^2}{2}(\mathcal{I} - \Gamma)K + \Delta t\Sigma^*\\
   v_{free}^{i} &= v^i - W^{-1}(\Delta tKq^i + (\Delta t^2\theta K + \Delta tC) v^i) & = v^i - W^{-1}(\Delta tKq^i + 2\Delta t \Sigma^* v^i) \\
   \end{array}




Taylor expansions
^^^^^^^^^^^^^^^^^

For some values of the time step it may be necessary to use Taylor expansions of iteration matrix and :math:`\Delta t\sigma^*` to avoid convergence problems.

Those terms write:

.. math::

   \Delta t\sigma^* & = \Delta t \sigma + \frac{\Delta t^{3} \sigma}{12} \omega^{2} + \Delta t^{5} \left(\frac{\omega^{4} \sigma}{240} - \frac{\omega^{2} \sigma^{3}}{180}\right) \\
   & +  \Delta t^{7} \left(\frac{\omega^{6} \sigma}{6048} - \frac{\omega^{4} \sigma^{3}}{1512} + \frac{\omega^{2} \sigma^{5}}{1890}\right) + \mathcal{O}\left(\Delta t^{8}\right) \\
   \frac{1}{W_{kk}} &= 1 - \Delta t \sigma + \Delta t^{2} \left(- \frac{\omega^{2}}{12} + \frac{2 \sigma^{2}}{3}\right) \\
   &+\Delta t^{3} \left(\frac{\omega^{2} \sigma}{12} - \frac{\sigma^{3}}{3}\right) + \Delta t^{4} \left(\frac{\omega^{4}}{360} - \frac{\omega^{2} \sigma^{2}}{20} + \frac{2 \sigma^{4}}{15}\right)\\
   &+ \Delta t^{5} \left(- \frac{\omega^{4} \sigma}{360} + \frac{\omega^{2} \sigma^{3}}{45} - \frac{2 \sigma^{5}}{45}\right) \\
   &+  \Delta t^{6} \left(- \frac{\omega^{6}}{20160} + \frac{\omega^{4} \sigma^{2}}{630} - \frac{\omega^{2} \sigma^{4}}{126} + \frac{4 \sigma^{6}}{315}\right)\\
   &+  \Delta t^{7} \left(\frac{\omega^{6} \sigma}{20160} - \frac{\omega^{4} \sigma^{3}}{1512} + \frac{\omega^{2} \sigma^{5}}{420} - \frac{\sigma^{7}}{315}\right) + \mathcal{O}\left(\Delta t^{8}\right)\\


Non-smooth problem formulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


.. math::

   \dot y &= S_c \dot q \\
   P &= S_c^T\lambda

.. math::

   \dot y_c^{i+1} &= S_c \dot q^{i+1} \\
   P^{i+1} &= S_c^T\lambda^{i+1}


.. math::
   
   \dot y^{i+1} &= S_cv^{i} - S_cW^{-1}(\Delta tKq^i + 2\Delta t \Sigma^* v^i) + S_cW^{-1}S_c^T\lambda^{i+1} \\
           &= q_{LCP} + M_{LCP}\lambda^{i+1}

with

.. math::

   0 \leq \dot y^{i+1} \perp \lambda^{i+1} \geq 0
   
