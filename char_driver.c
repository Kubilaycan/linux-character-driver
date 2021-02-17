#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
// 1024 byte buyuklugunde bir buffer olusturur
static char device_buffer[BUFFER_SIZE];

int openCount = 0;
int closeCount = 0;
int placeholder = 0;

// Olusturulan driver'a okuma islemi yaparken calisacak fonksiyon
ssize_t character_driver_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset){
	int bytesRead;
	int bytesToRead = BUFFER_SIZE - *offset;

	// Driver dosyasının sonuna gelindigini kontrol eder
	if(bytesToRead == 0){
		printk(KERN_INFO "Reached the end of the file");
		return bytesToRead;
	}

	// Driver dosyasından okuma yapar ve buffer a yazar
	bytesRead = bytesToRead - copy_to_user(buffer, device_buffer + *offset, bytesToRead);
	printk(KERN_INFO "Reading with character driver. Reading %d bytes.\n", bytesRead);

	*offset += bytesRead;
	return bytesRead;
}

// Olusturulan driver'a yazma islemi yaparken calisacak fonksiyon
ssize_t character_driver_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
	int bytesToWrite;
	int bytesWritten;
	int bytesAvailable = BUFFER_SIZE - *offset - placeholder;

	// Driver da yeterli alan olup olmadığını kontrol eder
	if(bytesAvailable > length){
		bytesToWrite = length;
	}else{
		bytesToWrite = bytesAvailable;
	}

	// Veriyi driver dosyasına yazar. Yeterli alan yoksa alan bitene kadar yazar.
	bytesWritten = bytesToWrite - copy_from_user(device_buffer + *offset + placeholder, buffer, bytesToWrite);


	// Cihazda yer kalmadıysa uyarı verir
	if(bytesWritten == 0){
		printk(KERN_INFO"The device is out of space!\n");
	}else{
		*offset += bytesWritten;
		placeholder += bytesWritten;
		printk(KERN_INFO "Writing with character driver. Writing %s, %d bytes.\n", buffer, bytesWritten);
	}
	return bytesWritten;
}


// Olusturulan driver acilirken calisir
int character_driver_open(struct inode *pinode, struct file *pfile){
	openCount++;
	// Cihazin ne kadar kullanildigini yazdirir
	printk(KERN_INFO "Opening character driver. Opened %d times.\n", openCount);
	return 0;
}

// Olusturulan driver kapanirken calisir
int character_driver_close(struct inode *pinode, struct file *pfile){
	closeCount++;
	printk(KERN_INFO "Closing character driver. Closed %d times.\n", closeCount);
	return 0;
}

// Yazdigimiz fonksiyonlari file operations struct'ina baglar
struct file_operations character_driver_file_operations = {
	.owner = THIS_MODULE,
	.read = character_driver_read,
	.write = character_driver_write,
	.open = character_driver_open,
	.release = character_driver_close
};

// Driver in constructor'u
static int character_driver_init(void){
	printk(KERN_INFO "Initializing character driver.\n");
	register_chrdev(150, "character_driver", &character_driver_file_operations);
	return 0;
}

// Driver in destructor'u
static void character_driver_exit(void){
	printk(KERN_INFO "Exiting character driver.\n");
	unregister_chrdev(150, "character_driver");
}

module_init(character_driver_init);
module_exit(character_driver_exit);
