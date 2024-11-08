# tfm
This repository contains all the TFM documents and programs.

There is hardware implementations based on a hardware accelerator that uses “Boosting” type decision trees, trained with LightGBM and converted to hardware using Vitis HLS, followed by subsequent implementation with Vivado.

Block design PING PONG buffers implementation
![Block design1](vivado/ping_pong_burst/bd.png)

# Linux use example
[Go to Linux example implementation](linux_drivers/)

# Building the system
[Go to building the system](vivado/)

# Training the model
[Go to training the model](python_code/)