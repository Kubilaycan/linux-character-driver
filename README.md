# Linux Character Driver

## Create a node named "character_driver" with major 150 and minor 0:
```bash
sudo mknod -m 777 /dev/character_driver c 150 0
```

## Check if node successfully created:
```bash
ls -la /dev | grep character_driver
```

## To compile the code and install the driver:
```bash
make
sudo insmod char_driver.ko
```

## Check if device installed:
```bash
cat /proc/devices | grep character_driver
```

## To run test script:
```bash
./char_driver_test test
cat /dev/character_driver
```

## To remove the driver:
```bash
sudo rm /dev/character_driver
sudo rmmod char_driver
```