#!/bin/bash

# Verificar si se pasó un parámetro
if [ -z "$1" ]; then
  echo "Uso: $0 <número_de_dispositivo>"
  exit 1
fi

DEVICE="$1"

# Ejecutar los comandos con el número de dispositivo pasado como parámetro
sudo chmod +777 /sys/bus/pci/devices/$DEVICE/remove
echo 1 > /sys/bus/pci/devices/$DEVICE/remove
sudo chmod +777 /sys/bus/pci/rescan
echo 1 > /sys/bus/pci/rescan
sudo chmod +777 /sys/bus/pci/devices/$DEVICE/resource0