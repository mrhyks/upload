#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h> // sk_buff (Socket buffer)
#include <linux/if_packet.h> // include for packet info
#include <linux/if_ether.h> // ETH_P_ALL
#include <linux/ip.h> // ip_hdr (Hlavicka IP paketu)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Petr Ilgner");
MODULE_DESCRIPTION("Modul zaregistruje handler prijateho paketu a zobrazi jeho zdrojovou IP v kernel logu.");

struct packet_type netcounter;

int netcounter_packet_received(struct sk_buff * skb, struct net_device * dev, struct packet_type * pt, struct net_device * orig_dev) {

  if (skb->pkt_type == PACKET_HOST) { // PACKET_HOST jsou prichozi unicastove pakety (v <linux/if_packet.h)
    struct iphdr * ip_header = ip_hdr(skb);

    printk(KERN_DEBUG "Dev: %s ; 0x%.4X ; 0x%.4X \n", skb->dev->name, ntohs(skb->protocol), ip_header->protocol);
    printk(KERN_DEBUG "IP-SRC: %pI4 \n", & ip_header->saddr);
  }
  kfree_skb(skb);
  return 0;
}

static int netcounter_init(void) {

  netcounter.type = htons(ETH_P_IP); // ETH_P_IP jen jsou IP pakety (v <linux/if_ether.h>)
  netcounter.dev = dev_get_by_name(&init_net, "eth0"); // Device kde naslouchat, NULL znamena vsechno
  netcounter.func = netcounter_packet_received; // Funkce, ktere bude predany prijaty paket

  dev_add_pack(&netcounter); // Pridat vytvoreny packet handler sitovemu stacku Linuxu

  printk(KERN_INFO "NETCOUNTER: Modul zaveden\n");
  return 0;
}

static void netcounter_cleanup(void) {
  dev_remove_pack(&netcounter); // Odebrat packet handler z kernelu
  printk(KERN_INFO "NETCOUNTER: Modul ukoncen\n");
}

module_init(netcounter_init);
module_exit(netcounter_cleanup);
