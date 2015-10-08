/* String matching match for iptables
 *
 * (C) 2005 Pablo Neira Ayuso <pablo@eurodev.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/netfilter/x_tables.h>
#include <linux/ip.h>

#include "xtstring.h"
#include "textsearch.h"

#define pr_warning(fmt, ...) \
        printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)


MODULE_AUTHOR("Pablo Neira Ayuso <pablo@eurodev.net>");
MODULE_DESCRIPTION("Xtables: string-based matching");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_string");
MODULE_ALIAS("ip6t_string");

static bool
string_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_string_info *conf = par->matchinfo;
	struct ts_state state;
	bool invert;
	char payload[256];
	char *p1;



	memset(&state, 0, sizeof(struct ts_state));
	invert = conf->u.v1.flags & XT_STRING_FLAG_INVERT;
	/*
	return (skb_find_text((struct sk_buff *)skb, conf->from_offset,
			     conf->to_offset, conf->config, &state)
			     != UINT_MAX) ^ invert; */
	
	unsigned int textend = (skb_find_text((struct sk_buff *)skb, conf->from_offset,
                             conf->to_offset, conf->config, &state));
	if  ((textend!= UINT_MAX)^ invert){
		/*
		payload = strchr((void *) skb->data + textend, ' ');
		payload = '\0';
		*/
		
		memcpy(payload, (void *) skb->data + textend,255);
		if ((p1 = strchr(payload, '\n'))) {
			p1[0] = '\0';
		}else{
			payload[255] = '\0';
		}
		struct sk_buff *sb = skb;
		struct iphdr *iph;

		iph = ip_hdr(sb);
		u8 *pAddr = sb->mac_header;
		//pr_warning("des mac address %02x:%02x:%02x:%02x:%02x:%02x\n",pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);//mac address

		//pr_warning("src mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
				//pAddr[6], pAddr[7], pAddr[8], pAddr[9], pAddr[10], pAddr[11]);//mac address

		

		//pr_warning("DPI %s \n",payload);
		pr_warning("DPI %02x%02x%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%s \n",pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]
		,pAddr[6], pAddr[7], pAddr[8], pAddr[9], pAddr[10], pAddr[11]
		,payload);
		return true;
	}else{
		return false;
        
       }

}

#define STRING_TEXT_PRIV(m) ((struct xt_string_info *)(m))

static int string_mt_check(const struct xt_mtchk_param *par)
{
	struct xt_string_info *conf = par->matchinfo;
	struct ts_config *ts_conf;
	int flags = TS_AUTOLOAD;

	/* Damn, can't handle this case properly with iptables... */
	if (conf->from_offset > conf->to_offset)
		return -EINVAL;
	if (conf->algo[XT_STRING_MAX_ALGO_NAME_SIZE - 1] != '\0')
		return -EINVAL;
	if (conf->patlen > XT_STRING_MAX_PATTERN_SIZE)
		return -EINVAL;
	if (conf->u.v1.flags &
	    ~(XT_STRING_FLAG_IGNORECASE | XT_STRING_FLAG_INVERT))
		return -EINVAL;
	if (conf->u.v1.flags & XT_STRING_FLAG_IGNORECASE)
		flags |= TS_IGNORECASE;
	ts_conf = textsearch_prepare(conf->algo, conf->pattern, conf->patlen,
				     GFP_KERNEL, flags);
	if (IS_ERR(ts_conf))
		return PTR_ERR(ts_conf);

	conf->config = ts_conf;
	return 0;
}

static void string_mt_destroy(const struct xt_mtdtor_param *par)
{
	textsearch_destroy(STRING_TEXT_PRIV(par->matchinfo)->config);
}

static struct xt_match xt_string_mt_reg __read_mostly = {
	.name       = "string",
	.revision   = 1,
	.family     = NFPROTO_UNSPEC,
	.checkentry = string_mt_check,
	.match      = string_mt,
	.destroy    = string_mt_destroy,
	.matchsize  = sizeof(struct xt_string_info),
	.me         = THIS_MODULE,
};

static int __init string_mt_init(void)
{
	return xt_register_match(&xt_string_mt_reg);
}

static void __exit string_mt_exit(void)
{
	xt_unregister_match(&xt_string_mt_reg);
}

module_init(string_mt_init);
module_exit(string_mt_exit);
