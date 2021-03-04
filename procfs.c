#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE	30


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Petr Ilgner");
MODULE_DESCRIPTION("Modul ukazuje tvorbu souboru v /proc");


static struct proc_dir_entry* soubor, * adresar;
static int pocitadlo;


// funkce ktera zajistuje cteni z virt. souboru
static ssize_t pocitadlo_read(struct file* file, char __user* ubuf, size_t count, loff_t* ppos)
{
	int delka = 0;
	char buffer[BUFFER_SIZE];


	if (*ppos > 0 || count < BUFFER_SIZE)
		return 0;


	pocitadlo++;
	printk("Pocitadlo navyseno na %d.\n", pocitadlo);
	delka = sprintf(buffer, "POCITADLO: %d \n", pocitadlo);

	// funkce copy_to_user zajisti vraceni bufferu do souboru
	if (copy_to_user(ubuf, buffer, delka))
		return -EFAULT;


	*ppos = delka;
	return delka;
}

// funkce ktera zajistuje zapis do virt. souboru
static ssize_t pocitadlo_write(struct file* file, const char __user* ubuf, size_t count, loff_t* ppos)
{
	printk(KERN_DEBUG "Pocitadlo vyresetovano.\n");
	pocitadlo = 0;
	return -1;
}


// definice struktury operaci souboru v procfs
static struct file_operations pocitadlo_operations =
{
	.owner = THIS_MODULE,
	.read = pocitadlo_read,
	.write = pocitadlo_write
};

// inicializace modulu
static int pocitadlo_init(void)
{
	printk(KERN_DEBUG "Pocitadlo INIT.\n");
	adresar = proc_mkdir("bsos", NULL);
	soubor = proc_create("pocitadlo", 0660, adresar, &pocitadlo_operations);
	return 0;
}

// ukonceni modulu
static void pocitadlo_clean(void)
{
	printk(KERN_DEBUG "Pocitadlo Bye.\n");
	proc_remove(soubor);
	proc_remove(adresar);
}

module_init(pocitadlo_init);
module_exit(pocitadlo_clean);