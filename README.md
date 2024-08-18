# tfm
this repository contains all the TFM documents and programs

There are different hardware implementations based on a hardware accelerator based on “Boosting” type decision trees trained with lightgbm and converted to hardware with vitis hls and its subsequent implementation with vivado.

hardware implementations:


Block design MIG
![Block design1](vivado/MIG_version/bd.png)


Block design XDMA
![Block design2](vivado/dma/bd.png)

Block design MMIO
![Block design3](vivado/memory_mapped/bd.png)


├── C_code_vitis: Contains test and HLS code
│   ├── load_and_execute_model.c
│   ├── predict.c
│   └── predict.h
├── CMakeLists.txt
├── datasets: Datasets used for inference
│   ├── alzheimers_disease_data.csv
│   ├── alzheimers_processed_dataset.csv
│   ├── anemia.csv
│   ├── anemia_processed_dataset.csv
│   ├── diabetes.csv
│   ├── Heart_Attack.csv
│   ├── Lung_Cancer_processed_dataset.csv
│   └── Lung_Cancer_raw.csv
├── ips: Contains IPs required for Vivado block diagrams
│   ├── export.zip
│   └── pcie_axi_lite_IP.zip
├── linux_drivers: C code to drive the FPGA from PCIe
│   ├── common: Common functions for all C drivers
│   │   ├── common.c
│   │   └── common.h
│   ├── dma
│   │   ├── defines_dma.h
│   │   └── dma.c
│   ├── memory_mapped
│   │   ├── defines_memory_mapped.h
│   │   └── memory_mapped.c
│   ├── MIG_2_DDR3_DMA
│   │   ├── defines_dma.h
│   │   └── main.c
│   └── reload_resource.sh
├── python_code: Python code
│   ├── interface.py: A graphical interface attempt
│   └── train_model.py: Trains the model and exports a binary for inference
├── README.md
├── trained_models: Pre-trained models
│   ├── alzheimers.model
│   ├── anemia.model
│   ├── diabetes.model
│   ├── heart_attack.model
│   └── lung_cancer.model
└── vivado: Contains all block diagrams in TCL format for rebuilding
    ├── dma
    │   ├── bd.png
    │   ├── constrs_1
    │   │   └── new
    │   │       ├── early.xdc
    │   │       └── normal.xdc
    │   ├── dmadesign_1.tcl
    │   ├── README.md
    │   └── vhdl
    │       └── running_leds.vhd
    └── memory_mapped
        ├── bd.png
        ├── constrs_1
        │   └── new
        │       ├── early.xdc
        │       └── normal.xdc
        ├── design_1.tcl
        ├── README.md
        └── vhdl
            └── running_leds.vhd
