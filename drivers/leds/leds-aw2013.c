<<<<<<< HEAD
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <linux/leds.h>
#include <linux/init.h>

#include <linux/mutex.h>

enum led_colors{
LED_RED = 0,
LED_GREEN,
LED_BLUE,
LED_WHITE,
LED_YELLOW,
LED_CYAN,
LED_PURPLE,
LED_COLORS_MAX
};

struct rgb_info{
int blinking;
int brightness;
int on_time;
int off_time;
};

struct aw2013_led_data {
	struct led_classdev cdev;
	enum led_colors color;
};


struct aw2013_leds_priv {
	int num_leds;
	struct aw2013_led_data leds[];
};

struct aw2013_dev_data {
	struct i2c_client	*i2c;
	struct regulator *regulator;
	struct aw2013_leds_priv *leds_priv;
	struct rgb_info leds[LED_COLORS_MAX];
	enum led_colors current_color;
	struct mutex		led_lock;
	struct work_struct	set_color_work;
};


struct aw2013_led{
	const char *name;
	const char *default_trigger;
	unsigned retain_state_suspended:1;
	unsigned default_state:2;

};

static inline int sizeof_aw2013_leds_priv(int num_leds)
{
	return sizeof(struct aw2013_leds_priv) +
		(sizeof(struct aw2013_led_data) * num_leds);
}

static struct aw2013_dev_data *s_aw2013;
bool blink_frequency_adjust;

#define AW2013_I2C_NAME   "aw2013"

#define AW2013_RSTR		0x0
#define AW2013_GCR			0x01
#define AW2013_STATUS		0x02
#define AW2013_LEDE		0x30
#define AW2013_LCFG0		0x31
#define AW2013_LCFG1		0x32
#define AW2013_LCFG2		0x33
#define AW2013_PWM0		0x34
#define AW2013_PWM1		0x35
#define AW2013_PWM2		0x36
#define AW2013_LED0_T0		0x37
#define AW2013_LED0_T1		0x38
#define AW2013_LED0_T2		0x39
#define AW2013_LED1_T0		0x3A
#define AW2013_LED1_T1		0x3B
#define AW2013_LED1_T2		0x3C
#define AW2013_LED2_T0		0x3D
#define AW2013_LED2_T1		0x3E
#define AW2013_LED2_T2		0x3F
#define AW2013_ASR			0x77

#define Bre_Imax          0x72
#define Rise_t  0x02
#define Fall_t   0x02
#define Hold_time   0x04
#define Off_time      0x04
#define Delay_time   0x00
#define Period_Num  0x00
#define Imax_R 0x62
#define Imax_G 0x62
#define Imax_B 0x62

#define MAX_BRIGHTNESS_RED 255
#define MAX_BRIGHTNESS_GREEN 255
#define MAX_BRIGHTNESS_BLUE 255
extern u8 tp_color;

typedef unsigned char U8;
static int aw2013_debug_enable;
#define AW2013_DEBUG(format, args...) do { \
	if (aw2013_debug_enable) \
	{\
		printk(format,##args);\
	}\
} while (0)
struct i2c_client *aw2013_client;
static	int aw2013_pdata;
struct i2c_board_info aw2013_info = {
			.type = "aw2013",
			.addr = 0x45,
			.platform_data = &aw2013_pdata,
		};


static int aw2013_has_inited;


static int aw2013_i2c_write(unsigned char cmd, unsigned char data)
{
	int ret;
	ret = i2c_smbus_write_byte_data(s_aw2013->i2c, cmd, data);
	return ret;
}

enum led_colors devname_to_color(const char *dev_name)
{
	if (!strcmp(dev_name, "red")) {
		return LED_RED;
	} else if (!strcmp(dev_name, "green")) {
		return LED_GREEN;
	} else if (!strcmp(dev_name, "blue")) {
		return LED_BLUE;
	} else if (!strcmp(dev_name, "white")) {
		return LED_WHITE;
	} else if (!strcmp(dev_name, "yellow")) {
		return LED_YELLOW;
	} else if (!strcmp(dev_name, "cyan")) {
		return LED_CYAN;
	} else if (!strcmp(dev_name, "purple")) {
		return LED_PURPLE;
	}

	return LED_COLORS_MAX;
}

int aw2013_set_color_singlecolor(struct aw2013_dev_data *aw2013)
{
	unsigned char red_on = 0, green_on = 0, blue_on = 0;
	unsigned char blink_flag = 0;

	if (aw2013->leds[LED_RED].brightness)
		red_on = 1;
	if (aw2013->leds[LED_GREEN].brightness)
		green_on = 1;
	if (aw2013->leds[LED_BLUE].brightness)
		blue_on = 1;

	if (aw2013->leds[LED_RED].brightness > MAX_BRIGHTNESS_RED)
		aw2013->leds[LED_RED].brightness = MAX_BRIGHTNESS_RED;
	if (aw2013->leds[LED_GREEN].brightness > MAX_BRIGHTNESS_GREEN)
		aw2013->leds[LED_GREEN].brightness = MAX_BRIGHTNESS_GREEN;
	if (aw2013->leds[LED_BLUE].brightness > MAX_BRIGHTNESS_BLUE)
		aw2013->leds[LED_BLUE].brightness = MAX_BRIGHTNESS_BLUE;

	if (aw2013->leds[LED_RED].blinking || aw2013->leds[LED_GREEN].blinking || aw2013->leds[LED_BLUE].blinking) {
		blink_flag = 0x10;
		aw2013_i2c_write(0x0, 0x54);
	}

	if (red_on || green_on || blue_on) {
		if (0 == aw2013_has_inited) {
			aw2013_i2c_write(0x0, 0x55);
			aw2013_i2c_write(0x01, 0x1);
			mdelay(1);
			aw2013_has_inited = 1;
		}

		aw2013_i2c_write(0x01, 0xe1);
		printk("tp_color:%d", tp_color);
		switch (tp_color) {
		case 0x31:
			if (red_on)
				aw2013_i2c_write(0x31, blink_flag|0x63);
			if (green_on)
				aw2013_i2c_write(0x32, blink_flag|0x63);
			if (blue_on)
				aw2013_i2c_write(0x33, blink_flag|0x63);
			printk("tp_color is white\n");
			break;
		case 0x34:
			if (red_on)
				aw2013_i2c_write(0x31, blink_flag|0x63);
			if (green_on)
				aw2013_i2c_write(0x32, blink_flag|0x63);
			if (blue_on)
				aw2013_i2c_write(0x33, blink_flag|0x63);
			printk("tp_color is yellow\n");
			break;
		case 0x38:
			if (red_on)
				aw2013_i2c_write(0x31, blink_flag|0x63);
			if (green_on)
				aw2013_i2c_write(0x32, blink_flag|0x63);
			if (blue_on)
				aw2013_i2c_write(0x33, blink_flag|0x63);
			printk("tp_color is golden\n");
			break;
		default:
			if (red_on)
				aw2013_i2c_write(0x31, blink_flag|0x62);
			if (green_on)
				aw2013_i2c_write(0x32, blink_flag|0x62);
			if (blue_on)
				aw2013_i2c_write(0x33, blink_flag|0x62);
			printk("tp_color is black\n");
			break;
		}
		if (red_on)
			aw2013_i2c_write(0x34, aw2013->leds[LED_RED].brightness);
		if (green_on)
			aw2013_i2c_write(0x35, aw2013->leds[LED_GREEN].brightness);
		if (blue_on)
			aw2013_i2c_write(0x36, aw2013->leds[LED_BLUE].brightness);

	if (blink_frequency_adjust) {

		aw2013_i2c_write(0x37, Rise_t<<4 | aw2013->leds[LED_RED].blinking);
		aw2013_i2c_write(0x38, Fall_t<<4 | aw2013->leds[LED_RED].blinking);
		aw2013_i2c_write(0x39, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3a, Rise_t<<4 | aw2013->leds[LED_GREEN].blinking);
		aw2013_i2c_write(0x3b, Fall_t<<4 | aw2013->leds[LED_GREEN].blinking);
		aw2013_i2c_write(0x3c, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3d, Rise_t<<4 | aw2013->leds[LED_BLUE].blinking);
		aw2013_i2c_write(0x3e, Fall_t<<4 | aw2013->leds[LED_BLUE].blinking);
		aw2013_i2c_write(0x3f, Delay_time<<4 | Period_Num);
	} else {

		aw2013_i2c_write(0x37, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x38, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x39, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3a, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x3b, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x3c, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3d, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x3e, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x3f, Delay_time<<4 | Period_Num);
	}
		aw2013_i2c_write(0x30, blue_on<<2|green_on<<1|red_on);
		mdelay(1);

	} else {
		aw2013_i2c_write(0x01, 0);
		mdelay(1);
		aw2013_i2c_write(0x30, 0);
		aw2013_i2c_write(0x34, 0);
		aw2013_i2c_write(0x35, 0);
		aw2013_i2c_write(0x36, 0);
		mdelay(1);
		aw2013_has_inited = 0;
	}

	return 0;

}

int aw2013_set_color_multicolor(struct aw2013_dev_data *aw2013, enum led_colors color)
{
	unsigned char blink_flag = 0;

	if (aw2013->leds[color].brightness) {
		if (aw2013->leds[color].blinking)
			blink_flag = 0x10;

		if (0 == aw2013_has_inited) {

			aw2013_i2c_write(0x0, 0x55);
			aw2013_i2c_write(0x01, 0x1);
			mdelay(1);
			aw2013_has_inited = 1;
		}

		switch (color) {

		case LED_WHITE:

			aw2013_i2c_write(0x0, 0x54);
			aw2013_i2c_write(0x01, 0xe1);
			aw2013_i2c_write(0x31, blink_flag|0x61);
			aw2013_i2c_write(0x32, blink_flag|0x62);
			aw2013_i2c_write(0x33, blink_flag|0x62);
			aw2013_i2c_write(0x34, MAX_BRIGHTNESS_RED);
			aw2013_i2c_write(0x35, MAX_BRIGHTNESS_GREEN);
			aw2013_i2c_write(0x36, MAX_BRIGHTNESS_BLUE);
			aw2013_i2c_write(0x30, 0x7);
			break;
		case LED_YELLOW:

			aw2013_i2c_write(0x0, 0x54);
			aw2013_i2c_write(0x01, 0xe1);
			aw2013_i2c_write(0x31, blink_flag|0x62);
			aw2013_i2c_write(0x32, blink_flag|0x62);
			aw2013_i2c_write(0x33, 0);
			aw2013_i2c_write(0x34, MAX_BRIGHTNESS_RED);
			aw2013_i2c_write(0x35, MAX_BRIGHTNESS_GREEN);
			aw2013_i2c_write(0x36, 0);
			aw2013_i2c_write(0x30, 0x3);
			break;
		case LED_PURPLE:

			aw2013_i2c_write(0x0, 0x54);
			aw2013_i2c_write(0x01, 0xe1);
			aw2013_i2c_write(0x31, blink_flag|0x62);
			aw2013_i2c_write(0x32, 0);
			aw2013_i2c_write(0x33, blink_flag|0x62);
			aw2013_i2c_write(0x34, MAX_BRIGHTNESS_RED);
			aw2013_i2c_write(0x35, 0);
			aw2013_i2c_write(0x36, MAX_BRIGHTNESS_BLUE);
			aw2013_i2c_write(0x30, 0x5);

			break;
		case LED_CYAN:

			aw2013_i2c_write(0x0, 0x54);
			aw2013_i2c_write(0x01, 0xe1);
			aw2013_i2c_write(0x31, 0);
			aw2013_i2c_write(0x32, blink_flag|0x62);
			aw2013_i2c_write(0x33, blink_flag|0x62);
			aw2013_i2c_write(0x34, 0);
			aw2013_i2c_write(0x35, MAX_BRIGHTNESS_GREEN);
			aw2013_i2c_write(0x36, MAX_BRIGHTNESS_BLUE);
			aw2013_i2c_write(0x30, 0x6);

			break;
		default:
			break;
		}
		aw2013_i2c_write(0x37, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x38, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x39, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3a, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x3b, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x3c, Delay_time<<4 | Period_Num);

		aw2013_i2c_write(0x3d, Rise_t<<4 | Hold_time);
		aw2013_i2c_write(0x3e, Fall_t<<4 | Off_time);
		aw2013_i2c_write(0x3f, Delay_time<<4 | Period_Num);
	} else {


		aw2013_i2c_write(0x01, 0);
		mdelay(1);
		aw2013_i2c_write(0x30, 0);
		aw2013_i2c_write(0x34, 0);
		aw2013_i2c_write(0x35, 0);
		aw2013_i2c_write(0x36, 0);
		mdelay(1);
		aw2013_has_inited = 0;
	}

	return 0;
}


static void set_color_delayed(struct work_struct *ws)
{
	enum led_colors color;
	struct aw2013_dev_data *aw2013 =
		container_of(ws, struct aw2013_dev_data, set_color_work);

			mutex_lock(&s_aw2013->led_lock);
			color = aw2013->current_color;

			AW2013_DEBUG("aw2013 set_color_delayed color = %d, brightness = %d, blink = %d \n", color, aw2013->leds[color].brightness, aw2013->leds[color].blinking);

			if (color == LED_RED || color == LED_GREEN || color == LED_BLUE) {
				aw2013_set_color_singlecolor(aw2013);
			} else {
				aw2013_set_color_multicolor(aw2013, color);
			}
			mutex_unlock(&s_aw2013->led_lock);
}

int aw2013_set_color(struct aw2013_dev_data *aw2013, enum led_colors color)
{

	aw2013->current_color = color;
	schedule_work(&s_aw2013->set_color_work);

	return 0;

}

static void aw2013_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	enum led_colors color;
	color = devname_to_color(led_cdev->name);
	s_aw2013->leds[color].brightness = value;
	s_aw2013->leds[color].blinking = 0;
	aw2013_set_color(s_aw2013, color);
}

static int aw2013_led_blink_set(struct led_classdev *led_cdev,
	unsigned long *delay_on, unsigned long *delay_off)
{
	enum led_colors color;

	color = devname_to_color(led_cdev->name);
	s_aw2013->leds[color].blinking = 1;
	s_aw2013->leds[color].brightness = 255;
	s_aw2013->leds[color].on_time = *delay_on;
	s_aw2013->leds[color].off_time = *delay_off;

	led_cdev->brightness = s_aw2013->leds[color].brightness;
	aw2013_set_color(s_aw2013, color);

	return 0;
}


static int aw2013_i2c_check_device(
	struct i2c_client *client)
{
	int err;
	int retreive_count = 0;
	while (retreive_count++ < 5) {
		msleep(10);
		err = aw2013_i2c_write(AW2013_RSTR, 0x55);
		if (err == 0)
			break;
	}

	return err;
}

static int aw2013_power_up(struct aw2013_dev_data *pdata, struct i2c_client *client, bool enable)
{
	int err = -1;

	pdata->regulator = devm_regulator_get(&client->dev, "rgb_led");
	if (IS_ERR(pdata->regulator)) {
		dev_err(&client->dev, "regulator get failed\n");
		err = PTR_ERR(pdata->regulator);
		pdata->regulator = NULL;
		return err;
	}

	if (enable) {
		err = regulator_enable(pdata->regulator);
		msleep(100);
	} else {
		err = regulator_disable(pdata->regulator);
	}

	return err;
}


static ssize_t blink_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", led_cdev->brightness);
}


static ssize_t blink_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t count)
{
	unsigned long blinking;
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	enum led_colors color;

	ret = kstrtoul(buf, 10, &blinking);
	if (ret)
		return ret;

	color = devname_to_color(led_cdev->name);
	led_cdev->brightness =
	s_aw2013->leds[color].brightness = blinking ? 255 : 0;
	s_aw2013->leds[color].blinking = blinking;

	led_cdev->brightness = s_aw2013->leds[color].brightness;

	aw2013_set_color(s_aw2013, color);

	return count;
}
static DEVICE_ATTR(blink, 0664, blink_show, blink_store);


static int create_aw2013_led(const struct aw2013_led *template,
	struct aw2013_led_data *led_dat, struct device *parent,
	int (*blink_set)(unsigned, int, unsigned long *, unsigned long *))
{
	int ret;

	led_dat->cdev.name = template->name;
	led_dat->cdev.default_trigger = template->default_trigger;
	led_dat->color = devname_to_color(template->name);
	led_dat->cdev.brightness = 0;
	if (!template->retain_state_suspended)
		led_dat->cdev.flags |= LED_CORE_SUSPENDRESUME;

	led_dat->cdev.blink_set = aw2013_led_blink_set;
	led_dat->cdev.brightness_set = aw2013_led_set;

	ret = led_classdev_register(parent, &led_dat->cdev);

	device_create_file(led_dat->cdev.dev, &dev_attr_blink);

	if (ret < 0)
		return ret;

	return 0;
}

static void delete_aw2013_led(struct aw2013_led_data *led)
{
	device_remove_file(led->cdev.dev, &dev_attr_blink);
	led_classdev_unregister(&led->cdev);
}

static struct aw2013_leds_priv *aw2013_leds_create_of(struct i2c_client *client)
{
	struct device_node *np = client->dev.of_node;
	struct device_node *child;
	struct aw2013_leds_priv *priv;
	int count, ret;

	/* count LEDs in this device, so we know how much to allocate */
	count = of_get_child_count(np);
	if (!count)
		return ERR_PTR(-ENODEV);

	priv = devm_kzalloc(&client->dev, sizeof_aw2013_leds_priv(count),
			GFP_KERNEL);
	if (!priv)
		return ERR_PTR(-ENOMEM);

	for_each_child_of_node(np, child) {
		struct aw2013_led led = {};

		led.name = of_get_property(child, "label", NULL) ? : child->name;
		led.default_trigger =
			of_get_property(child, "linux,default-trigger", NULL);
		led.retain_state_suspended =
			(unsigned)of_property_read_bool(child,
				"retain-state-suspended");

		ret = create_aw2013_led(&led, &priv->leds[priv->num_leds++],
				      &client->dev, NULL);
		if (ret < 0) {
			of_node_put(child);
			goto err;
		}
	}

	return priv;

err:
	for (count = priv->num_leds - 2; count >= 0; count--)
		delete_aw2013_led(&priv->leds[count]);

	devm_kfree(&client->dev, priv);

	return ERR_PTR(-ENODEV);
}


static int aw2013_led_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;
	struct aw2013_leds_priv *priv;
	printk("aw2013_led_probe start\n");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
				"%s: check_functionality failed.", __func__);
		err = -ENODEV;
		goto exit0;
	}

	/* Allocate memory for driver data */
	s_aw2013 = kzalloc(sizeof(struct aw2013_dev_data), GFP_KERNEL);
	if (!s_aw2013) {
		dev_err(&client->dev,
				"%s: memory allocation failed.", __func__);
		err = -ENOMEM;
		goto exit1;
	}

	/***** I2C initialization *****/
	s_aw2013->i2c = client;
	/* set client data */
	i2c_set_clientdata(client, s_aw2013);

	mutex_init(&s_aw2013->led_lock);

	INIT_WORK(&s_aw2013->set_color_work, set_color_delayed);

	if (0 != aw2013_power_up(s_aw2013, client, true)) {
		goto exit2;
	}
	if (0 != aw2013_i2c_check_device(client)) {
		dev_err(&client->dev,
				"%s:  aw2013_i2c_check_device failed.", __func__);
		goto exit2;
	}

	blink_frequency_adjust = of_property_read_bool(client->dev.of_node, "blink-frequency-adjustable");

	priv = aw2013_leds_create_of(client);

	if (IS_ERR(priv))
		goto exit3;

	s_aw2013->leds_priv = priv;

	return 0;

exit3:
exit2:
	kfree(s_aw2013);
exit1:
exit0:
	return err;
=======
/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/regulator/consumer.h>
#include <linux/leds-aw2013.h>

/* register address */
#define AW_REG_RESET			0x00
#define AW_REG_GLOBAL_CONTROL		0x01
#define AW_REG_LED_STATUS		0x02
#define AW_REG_LED_ENABLE		0x30
#define AW_REG_LED_CONFIG_BASE		0x31
#define AW_REG_LED_BRIGHTNESS_BASE	0x34
#define AW_REG_TIMESET0_BASE		0x37
#define AW_REG_TIMESET1_BASE		0x38

/* register bits */
#define AW2013_CHIPID			0x33
#define AW_LED_MOUDLE_ENABLE_MASK	0x01
#define AW_LED_FADE_OFF_MASK		0x40
#define AW_LED_FADE_ON_MASK		0x20
#define AW_LED_BREATHE_MODE_MASK	0x10
#define AW_LED_RESET_MASK		0x55

#define AW_LED_RESET_DELAY		8
#define AW2013_VDD_MIN_UV		2600000
#define AW2013_VDD_MAX_UV		3300000
#define AW2013_VI2C_MIN_UV		1800000
#define AW2013_VI2C_MAX_UV		1800000

#define MAX_RISE_TIME_MS		7
#define MAX_HOLD_TIME_MS		5
#define MAX_FALL_TIME_MS		7
#define MAX_OFF_TIME_MS			5

struct aw2013_led {
	struct i2c_client *client;
	struct led_classdev cdev;
	struct aw2013_platform_data *pdata;
	struct work_struct brightness_work;
	struct mutex lock;
	struct regulator *vdd;
	struct regulator *vcc;
	int num_leds;
	int id;
	bool poweron;
};

static int aw2013_write(struct aw2013_led *led, u8 reg, u8 val)
{
	return i2c_smbus_write_byte_data(led->client, reg, val);
}

static int aw2013_read(struct aw2013_led *led, u8 reg, u8 *val)
{
	s32 ret;

	ret = i2c_smbus_read_byte_data(led->client, reg);
	if (ret < 0)
		return ret;

	*val = ret;
	return 0;
}

static int aw2013_power_on(struct aw2013_led *led, bool on)
{
	int rc;

	if (on) {
		rc = regulator_enable(led->vdd);
		if (rc) {
			dev_err(&led->client->dev,
				"Regulator vdd enable failed rc=%d\n", rc);
			return rc;
		}

		rc = regulator_enable(led->vcc);
		if (rc) {
			dev_err(&led->client->dev,
				"Regulator vcc enable failed rc=%d\n", rc);
			goto fail_enable_reg;
		}
		led->poweron = true;
	} else {
		rc = regulator_disable(led->vdd);
		if (rc) {
			dev_err(&led->client->dev,
				"Regulator vdd disable failed rc=%d\n", rc);
			return rc;
		}

		rc = regulator_disable(led->vcc);
		if (rc) {
			dev_err(&led->client->dev,
				"Regulator vcc disable failed rc=%d\n", rc);
			goto fail_disable_reg;
		}
		led->poweron = false;
	}
	return rc;

fail_enable_reg:
	rc = regulator_disable(led->vdd);
	if (rc)
		dev_err(&led->client->dev,
			"Regulator vdd disable failed rc=%d\n", rc);

	return rc;

fail_disable_reg:
	rc = regulator_enable(led->vdd);
	if (rc)
		dev_err(&led->client->dev,
			"Regulator vdd enable failed rc=%d\n", rc);

	return rc;
}

static int aw2013_power_init(struct aw2013_led *led, bool on)
{
	int rc;

	if (on) {
		led->vdd = regulator_get(&led->client->dev, "vdd");
		if (IS_ERR(led->vdd)) {
			rc = PTR_ERR(led->vdd);
			dev_err(&led->client->dev,
				"Regulator get failed vdd rc=%d\n", rc);
			return rc;
		}

		if (regulator_count_voltages(led->vdd) > 0) {
			rc = regulator_set_voltage(led->vdd, AW2013_VDD_MIN_UV,
						   AW2013_VDD_MAX_UV);
			if (rc) {
				dev_err(&led->client->dev,
					"Regulator set_vtg failed vdd rc=%d\n",
					rc);
				goto reg_vdd_put;
			}
		}

		led->vcc = regulator_get(&led->client->dev, "vcc");
		if (IS_ERR(led->vcc)) {
			rc = PTR_ERR(led->vcc);
			dev_err(&led->client->dev,
				"Regulator get failed vcc rc=%d\n", rc);
			goto reg_vdd_set_vtg;
		}

		if (regulator_count_voltages(led->vcc) > 0) {
			rc = regulator_set_voltage(led->vcc, AW2013_VI2C_MIN_UV,
						   AW2013_VI2C_MAX_UV);
			if (rc) {
				dev_err(&led->client->dev,
				"Regulator set_vtg failed vcc rc=%d\n", rc);
				goto reg_vcc_put;
			}
		}
	} else {
		if (regulator_count_voltages(led->vdd) > 0)
			regulator_set_voltage(led->vdd, 0, AW2013_VDD_MAX_UV);

		regulator_put(led->vdd);

		if (regulator_count_voltages(led->vcc) > 0)
			regulator_set_voltage(led->vcc, 0, AW2013_VI2C_MAX_UV);

		regulator_put(led->vcc);
	}
	return 0;

reg_vcc_put:
	regulator_put(led->vcc);
reg_vdd_set_vtg:
	if (regulator_count_voltages(led->vdd) > 0)
		regulator_set_voltage(led->vdd, 0, AW2013_VDD_MAX_UV);
reg_vdd_put:
	regulator_put(led->vdd);
	return rc;
}

static void aw2013_brightness_work(struct work_struct *work)
{
	struct aw2013_led *led = container_of(work, struct aw2013_led,
					brightness_work);
	u8 val;

	mutex_lock(&led->pdata->led->lock);

	/* enable regulators if they are disabled */
	if (!led->pdata->led->poweron) {
		if (aw2013_power_on(led->pdata->led, true)) {
			dev_err(&led->pdata->led->client->dev, "power on failed");
			mutex_unlock(&led->pdata->led->lock);
			return;
		}
	}

	if (led->cdev.brightness > 0) {
		if (led->cdev.brightness > led->cdev.max_brightness)
			led->cdev.brightness = led->cdev.max_brightness;
		aw2013_write(led, AW_REG_GLOBAL_CONTROL,
			AW_LED_MOUDLE_ENABLE_MASK);
		aw2013_write(led, AW_REG_LED_CONFIG_BASE + led->id,
			led->pdata->max_current);
		aw2013_write(led, AW_REG_LED_BRIGHTNESS_BASE + led->id,
			led->cdev.brightness);
		aw2013_read(led, AW_REG_LED_ENABLE, &val);
		aw2013_write(led, AW_REG_LED_ENABLE, val | (1 << led->id));
	} else {
		aw2013_read(led, AW_REG_LED_ENABLE, &val);
		aw2013_write(led, AW_REG_LED_ENABLE, val & (~(1 << led->id)));
	}

	aw2013_read(led, AW_REG_LED_ENABLE, &val);
	/*
	 * If value in AW_REG_LED_ENABLE is 0, it means the RGB leds are
	 * all off. So we need to power it off.
	 */
	if (val == 0) {
		if (aw2013_power_on(led->pdata->led, false)) {
			dev_err(&led->pdata->led->client->dev,
				"power off failed");
			mutex_unlock(&led->pdata->led->lock);
			return;
		}
	}

	mutex_unlock(&led->pdata->led->lock);
}

static void aw2013_led_blink_set(struct aw2013_led *led, unsigned long blinking)
{
	u8 val;

	/* enable regulators if they are disabled */
	if (!led->pdata->led->poweron) {
		if (aw2013_power_on(led->pdata->led, true)) {
			dev_err(&led->pdata->led->client->dev, "power on failed");
			return;
		}
	}

	led->cdev.brightness = blinking ? led->cdev.max_brightness : 0;

	if (blinking > 0) {
		aw2013_write(led, AW_REG_GLOBAL_CONTROL,
			AW_LED_MOUDLE_ENABLE_MASK);
		aw2013_write(led, AW_REG_LED_CONFIG_BASE + led->id,
			AW_LED_FADE_OFF_MASK | AW_LED_FADE_ON_MASK |
			AW_LED_BREATHE_MODE_MASK | led->pdata->max_current);
		aw2013_write(led, AW_REG_LED_BRIGHTNESS_BASE + led->id,
			led->cdev.brightness);
		aw2013_write(led, AW_REG_TIMESET0_BASE + led->id * 3,
			led->pdata->rise_time_ms << 4 |
			led->pdata->hold_time_ms);
		aw2013_write(led, AW_REG_TIMESET1_BASE + led->id * 3,
			led->pdata->fall_time_ms << 4 |
			led->pdata->off_time_ms);
		aw2013_read(led, AW_REG_LED_ENABLE, &val);
		aw2013_write(led, AW_REG_LED_ENABLE, val | (1 << led->id));
	} else {
		aw2013_read(led, AW_REG_LED_ENABLE, &val);
		aw2013_write(led, AW_REG_LED_ENABLE, val & (~(1 << led->id)));
	}

	aw2013_read(led, AW_REG_LED_ENABLE, &val);
	/*
	 * If value in AW_REG_LED_ENABLE is 0, it means the RGB leds are
	 * all off. So we need to power it off.
	 */
	if (val == 0) {
		if (aw2013_power_on(led->pdata->led, false)) {
			dev_err(&led->pdata->led->client->dev,
				"power off failed");
			return;
		}
	}
}

static void aw2013_set_brightness(struct led_classdev *cdev,
			     enum led_brightness brightness)
{
	struct aw2013_led *led = container_of(cdev, struct aw2013_led, cdev);

	led->cdev.brightness = brightness;

	schedule_work(&led->brightness_work);
}

static ssize_t aw2013_store_blink(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t len)
{
	unsigned long blinking;
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct aw2013_led *led =
			container_of(led_cdev, struct aw2013_led, cdev);
	ssize_t ret = -EINVAL;

	ret = kstrtoul(buf, 10, &blinking);
	if (ret)
		return ret;
	mutex_lock(&led->pdata->led->lock);
	aw2013_led_blink_set(led, blinking);
	mutex_unlock(&led->pdata->led->lock);

	return len;
}

static ssize_t aw2013_led_time_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct aw2013_led *led =
			container_of(led_cdev, struct aw2013_led, cdev);

	return snprintf(buf, PAGE_SIZE, "%d %d %d %d\n",
			led->pdata->rise_time_ms, led->pdata->hold_time_ms,
			led->pdata->fall_time_ms, led->pdata->off_time_ms);
}

static ssize_t aw2013_led_time_store(struct device *dev,
			     struct device_attribute *attr,
			     const char *buf, size_t len)
{
	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	struct aw2013_led *led =
			container_of(led_cdev, struct aw2013_led, cdev);
	int rc, rise_time_ms, hold_time_ms, fall_time_ms, off_time_ms;

	rc = sscanf(buf, "%d %d %d %d",
			&rise_time_ms, &hold_time_ms,
			&fall_time_ms, &off_time_ms);

	mutex_lock(&led->pdata->led->lock);
	led->pdata->rise_time_ms = (rise_time_ms > MAX_RISE_TIME_MS) ?
				MAX_RISE_TIME_MS : rise_time_ms;
	led->pdata->hold_time_ms = (hold_time_ms > MAX_HOLD_TIME_MS) ?
				MAX_HOLD_TIME_MS : hold_time_ms;
	led->pdata->fall_time_ms = (fall_time_ms > MAX_FALL_TIME_MS) ?
				MAX_FALL_TIME_MS : fall_time_ms;
	led->pdata->off_time_ms = (off_time_ms > MAX_OFF_TIME_MS) ?
				MAX_OFF_TIME_MS : off_time_ms;
	aw2013_led_blink_set(led, 1);
	mutex_unlock(&led->pdata->led->lock);
	return len;
}

static DEVICE_ATTR(blink, 0664, NULL, aw2013_store_blink);
static DEVICE_ATTR(led_time, 0664, aw2013_led_time_show, aw2013_led_time_store);

static struct attribute *aw2013_led_attributes[] = {
	&dev_attr_blink.attr,
	&dev_attr_led_time.attr,
	NULL,
};

static struct attribute_group aw2013_led_attr_group = {
	.attrs = aw2013_led_attributes
};

static int aw_2013_check_chipid(struct aw2013_led *led)
{
	u8 val;

	aw2013_write(led, AW_REG_RESET, AW_LED_RESET_MASK);
	udelay(AW_LED_RESET_DELAY);
	aw2013_read(led, AW_REG_RESET, &val);
	if (val == AW2013_CHIPID)
		return 0;
	else
		return -EINVAL;
}

static int aw2013_led_err_handle(struct aw2013_led *led_array,
				int parsed_leds)
{
	int i;
	/*
	 * If probe fails, cannot free resource of all LEDs, only free
	 * resources of LEDs which have allocated these resource really.
	 */
	for (i = 0; i < parsed_leds; i++) {
		sysfs_remove_group(&led_array[i].cdev.dev->kobj,
				&aw2013_led_attr_group);
		led_classdev_unregister(&led_array[i].cdev);
		cancel_work_sync(&led_array[i].brightness_work);
		devm_kfree(&led_array->client->dev, led_array[i].pdata);
		led_array[i].pdata = NULL;
	}
	return i;
}

static int aw2013_led_parse_child_node(struct aw2013_led *led_array,
				struct device_node *node)
{
	struct aw2013_led *led;
	struct device_node *temp;
	struct aw2013_platform_data *pdata;
	int rc = 0, parsed_leds = 0;

	for_each_child_of_node(node, temp) {
		led = &led_array[parsed_leds];
		led->client = led_array->client;

		pdata = devm_kzalloc(&led->client->dev,
				sizeof(struct aw2013_platform_data),
				GFP_KERNEL);
		if (!pdata) {
			dev_err(&led->client->dev,
				"Failed to allocate memory\n");
			goto free_err;
		}
		pdata->led = led_array;
		led->pdata = pdata;

		rc = of_property_read_string(temp, "aw2013,name",
			&led->cdev.name);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading led name, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,id",
			&led->id);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading id, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,max-brightness",
			&led->cdev.max_brightness);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading max-brightness, rc = %d\n",
				rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,max-current",
			&led->pdata->max_current);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading max-current, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,rise-time-ms",
			&led->pdata->rise_time_ms);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading rise-time-ms, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,hold-time-ms",
			&led->pdata->hold_time_ms);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading hold-time-ms, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,fall-time-ms",
			&led->pdata->fall_time_ms);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading fall-time-ms, rc = %d\n", rc);
			goto free_pdata;
		}

		rc = of_property_read_u32(temp, "aw2013,off-time-ms",
			&led->pdata->off_time_ms);
		if (rc < 0) {
			dev_err(&led->client->dev,
				"Failure reading off-time-ms, rc = %d\n", rc);
			goto free_pdata;
		}

		INIT_WORK(&led->brightness_work, aw2013_brightness_work);

		led->cdev.brightness_set = aw2013_set_brightness;

		rc = led_classdev_register(&led->client->dev, &led->cdev);
		if (rc) {
			dev_err(&led->client->dev,
				"unable to register led %d,rc=%d\n",
				led->id, rc);
			goto free_pdata;
		}

		rc = sysfs_create_group(&led->cdev.dev->kobj,
				&aw2013_led_attr_group);
		if (rc) {
			dev_err(&led->client->dev, "led sysfs rc: %d\n", rc);
			goto free_class;
		}
		parsed_leds++;
	}

	return 0;

free_class:
	aw2013_led_err_handle(led_array, parsed_leds);
	led_classdev_unregister(&led_array[parsed_leds].cdev);
	cancel_work_sync(&led_array[parsed_leds].brightness_work);
	devm_kfree(&led->client->dev, led_array[parsed_leds].pdata);
	led_array[parsed_leds].pdata = NULL;
	return rc;

free_pdata:
	aw2013_led_err_handle(led_array, parsed_leds);
	devm_kfree(&led->client->dev, led_array[parsed_leds].pdata);
	return rc;

free_err:
	aw2013_led_err_handle(led_array, parsed_leds);
	return rc;
}

static int aw2013_led_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct aw2013_led *led_array;
	struct device_node *node;
	int ret, num_leds = 0;

	node = client->dev.of_node;
	if (node == NULL)
		return -EINVAL;

	num_leds = of_get_child_count(node);

	if (!num_leds)
		return -EINVAL;

	led_array = devm_kzalloc(&client->dev,
			(sizeof(struct aw2013_led) * num_leds), GFP_KERNEL);
	if (!led_array)
		return -ENOMEM;

	led_array->client = client;
	led_array->num_leds = num_leds;

	mutex_init(&led_array->lock);

	ret = aw_2013_check_chipid(led_array);
	if (ret) {
		dev_err(&client->dev, "Check chip id error\n");
		goto free_led_arry;
	}

	ret = aw2013_led_parse_child_node(led_array, node);
	if (ret) {
		dev_err(&client->dev, "parsed node error\n");
		goto free_led_arry;
	}

	i2c_set_clientdata(client, led_array);

	ret = aw2013_power_init(led_array, true);
	if (ret) {
		dev_err(&client->dev, "power init failed");
		goto fail_parsed_node;
	}

	return 0;

fail_parsed_node:
	aw2013_led_err_handle(led_array, num_leds);
free_led_arry:
	mutex_destroy(&led_array->lock);
	devm_kfree(&client->dev, led_array);
	led_array = NULL;
	return ret;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
}

static int aw2013_led_remove(struct i2c_client *client)
{
<<<<<<< HEAD
	int count = 0;
	struct aw2013_dev_data *aw2013 = i2c_get_clientdata(client);

	for (count = aw2013->leds_priv->num_leds - 2; count >= 0; count--)
	delete_aw2013_led(&(aw2013->leds_priv->leds[count]));

	devm_kfree(&client->dev, aw2013->leds_priv);

	kfree(aw2013);
	dev_info(&client->dev, "successfully removed.");
=======
	struct aw2013_led *led_array = i2c_get_clientdata(client);
	int i, parsed_leds = led_array->num_leds;

	for (i = 0; i < parsed_leds; i++) {
		sysfs_remove_group(&led_array[i].cdev.dev->kobj,
				&aw2013_led_attr_group);
		led_classdev_unregister(&led_array[i].cdev);
		cancel_work_sync(&led_array[i].brightness_work);
		devm_kfree(&client->dev, led_array[i].pdata);
		led_array[i].pdata = NULL;
	}
	mutex_destroy(&led_array->lock);
	devm_kfree(&client->dev, led_array);
	led_array = NULL;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	return 0;
}

static const struct i2c_device_id aw2013_led_id[] = {
<<<<<<< HEAD
	{AW2013_I2C_NAME, 0 },
	{ }
};

static struct of_device_id aw2013_led_match_table[] = {
	{ .compatible = "awinc,aw2013", },
=======
	{"aw2013_led", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, aw2013_led_id);

static struct of_device_id aw2013_match_table[] = {
	{ .compatible = "awinic,aw2013",},
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	{ },
};

static struct i2c_driver aw2013_led_driver = {
<<<<<<< HEAD
	.probe		= aw2013_led_probe,
	.remove		= aw2013_led_remove,
	.id_table	= aw2013_led_id,
	.driver = {
		.name	= AW2013_I2C_NAME,
		.owner  = THIS_MODULE,
		.of_match_table = aw2013_led_match_table,
	},
=======
	.probe = aw2013_led_probe,
	.remove = aw2013_led_remove,
	.driver = {
		.name = "aw2013_led",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(aw2013_match_table),
	},
	.id_table = aw2013_led_id,
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static int __init aw2013_led_init(void)
{
<<<<<<< HEAD
	pr_info("aw2013 led driver: initialize.");
	return i2c_add_driver(&aw2013_led_driver);
}

static void __exit aw2013_led_exit(void)
{
	pr_info("aw2013 led driver: release.");
	i2c_del_driver(&aw2013_led_driver);
}

subsys_initcall(aw2013_led_init);
module_exit(aw2013_led_exit);

MODULE_DESCRIPTION("aw2013 driver");
MODULE_LICENSE("GPL");
=======
	return i2c_add_driver(&aw2013_led_driver);
}
module_init(aw2013_led_init);

static void __exit aw2013_led_exit(void)
{
	i2c_del_driver(&aw2013_led_driver);
}
module_exit(aw2013_led_exit);

MODULE_DESCRIPTION("AWINIC aw2013 LED driver");
MODULE_LICENSE("GPL v2");
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
