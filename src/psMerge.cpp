// ========================================
// Author: wuxiaomeng
// Date: 2021-10
// nuke���, ����ģ��Photoshopͼ����ģʽ.
// ========================================
#pragma warning(disable: 4996)
#pragma execution_character_set("UTF-8")
#include <iostream>
#include <cmath>
#include <vector>
#include "DDImage/PixelIop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"

#define ARGB_LEVER 255.0f
typedef float argb;
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

using namespace DD;
using namespace DD::Image;

namespace photoshopMergeTool {
	enum PsBlend {
		Normal, Darken, Multiply, ColorBurn, LinearBurn, DarkerColor, Lighten, Screen, ColorDodge, LinearDodge, LighterColor,
		Overlay, SoftLight, HardLight, VividLight, LinearLight, PinLight, HardMix, Diference, Exclusion,
		Hue, Saturation, Color, Luminosity, Dissolve
	};

	enum PsMode {
		asValueBlend, asColorBlend
	};

	class PhotoshopComput
	{
	public:
		PhotoshopComput() = default;
		~PhotoshopComput() = default;
		static float clump_to_ps_argb(float);
		static float convert_from_argb(argb);
		static argb inverted(argb);  // ����
		static float normal(float, float);  // ����
		static float darken(float, float);    // �䰵
		static float multiply(float, float);  // ��Ƭ����
		static float color_burn(float, float); // ��ɫ����
		static float linear_burn(float, float); // ���Լ���
		static std::vector <float> PhotoshopComput::darker_color(std::vector <float> a, std::vector <float> b); // ��ɫ
		static float lighten(float, float);    // ����
		static float screen(float, float); // ��ɫ
		static float color_dodge(float, float); // ��ɫ����
		static float linear_dodge(float, float); // ���Լ���
		static std::vector <float> PhotoshopComput::lighter_color(std::vector <float> a, std::vector <float> b); // ǳɫ
		static float overlay(float, float); // ����
		static float soft_light(float, float); // ���
		static float hard_light(float, float); // ǿ��
		static float vivid_light(float, float); // ����
		static float linear_light(float, float); // ���Թ�
		static float pin_light(float, float); // ���
		static float hard_mix(float, float); // ʵɫ���
		static float diference(float, float); // �ų�
		static float exclusion(float, float); // ��ֵ
		static std::vector <float> PhotoshopComput::hue(std::vector <float> a, std::vector <float> b); // ɫ��
	};

	float PhotoshopComput::clump_to_ps_argb(float a)
	{
		argb r = (argb)(a * ARGB_LEVER);
		return r;
	}

	float PhotoshopComput::convert_from_argb(argb a)
	{
		float r = float(a) / ARGB_LEVER;
		return r;
	}

	argb PhotoshopComput::inverted(argb a)
	{
		argb r = (argb)(ARGB_LEVER - a);
		return r;
	}

	float PhotoshopComput::normal(float a, float b)
	{
		// ����
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		float r = PhotoshopComput::convert_from_argb(r_b);
		return b;
	}

	float PhotoshopComput::darken(float a, float b)
	{
		// �䰵   B<=A �� C=B B>=A �� C=A
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_a >= r_b)
			r_1 = r_b;
		else
			r_1 = r_a;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::multiply(float a, float b)
	{
		// ��Ƭ����    C = (A*B)/255
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = r_a * r_b / ARGB_LEVER;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::color_burn(float a, float b)
	{
		// ��ɫ����    C = A - ( (255 - A) * (255 - B) ) / B
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		if (r_b == 0)
			return 0.0;
		argb r_1 = r_a - PhotoshopComput::inverted(r_a) * PhotoshopComput::inverted(r_b) / r_b;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::linear_burn(float a, float b)
	{
		// ���Լ���   C=A+B-255
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = r_a + r_b - ARGB_LEVER;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::lighten(float a, float b)
	{
		// ���� B<=A �� C=A B>A �� C=B
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_a >= r_b)
			r_1 = r_a;
		else
			r_1 = r_b;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::screen(float a, float b)
	{
		// ��ɫ C = 255 - ( (255 - A) * (255 - B) ) / 255
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		r_1 = ARGB_LEVER - PhotoshopComput::inverted(r_a) * PhotoshopComput::inverted(r_b) / ARGB_LEVER;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::color_dodge(float a, float b)
	{
		// ��ɫ����   C = A + (A * B) / (255 - B)
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		if (PhotoshopComput::inverted(r_b) == 0)
			return 1.0;
		argb r_1 = r_a + r_a * r_b / PhotoshopComput::inverted(r_b);
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::linear_dodge(float a, float b)
	{
		// ���Լ�������ӣ�   C=A+B
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = r_a + r_b;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::overlay(float a, float b)
	{
		// ����  if A <= 128 �� C = ( A �� B ) / 255, if A > 128 �� C = 255 - ( (255 - A) * (255 - B)) / 128
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_a <= 128)
			r_1 = r_a * r_b / 128;
		else
			r_1 = 255 - PhotoshopComput::inverted(r_a) * PhotoshopComput::inverted(r_b) / 128;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::soft_light(float a, float b)
	{
		// ���    if B <= 128 �� C= (A * B) / 128 + (A / 255) ^ 2��(255 - 2B) , if B>128 �� C = ( A * ( 255 - _B ) ) / 128 + sqrt(A / 255) * (2B - 255)
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_b <= 128)
			r_1 = r_a * r_b / 128 + (argb)(pow((r_a / 255), 2)) * (255 - 2 * r_b);
		else
			r_1 = r_a * PhotoshopComput::inverted(r_b) / 128 + (argb)(sqrt((r_a / 255))) * (2 * r_b - 255);
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::hard_light(float a, float b)
	{
		// ǿ��  if B<=128 �� C=(A * B)/128 , if B>128 �� C = 255 - ( (255 - A) * (255 - B) ) / 128
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_b <= 128)
			r_1 = r_a * r_b / 128;
		else
			r_1 = 255 - PhotoshopComput::inverted(r_a) * PhotoshopComput::inverted(r_b) / 128;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::vivid_light(float a, float b)
	{
		// ����   if B <= 128 �� C = A - (255 - A) * (255-2B) / (2B), if  B>128 �� C = A + A * (2B - 255) / (2 * (255 - B) )
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_b == 0)
			return 0.0;
		if (PhotoshopComput::inverted(r_b) == 0)
			return 0.0;
		if (r_b <= 128)
			r_1 = r_a - PhotoshopComput::inverted(r_a) * (255 - 2 * r_b) / (2 * r_b);
		else
			r_1 = r_a + r_a * (2 * r_b - 255) / (2 * PhotoshopComput::inverted(r_b));
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::linear_light(float a, float b)
	{
		// ���Թ�  C = A + 2 * B - 255
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		r_1 = r_a + 2 * r_b - 255;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::pin_light(float a, float b)
	{
		// ���  if B <= 128 �� C = Min(A, 2B),  if B>128 �� C = MAX(A, 2B - 255)
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_b <= 128)
			r_1 = min(r_a, 2 * r_b);
		else
			r_1 = max(r_a, 2 * r_b - 255);
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::hard_mix(float a, float b)
	{
		// ʵɫ���   A + B >= 255 �� C=255, else C = A + B
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		if (r_a + r_b > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		else
			r_1 = 0;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::diference(float a, float b)
	{
		// ��ֵ   C = | A - B |
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		r_1 = abs(r_a - r_b);
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	float PhotoshopComput::exclusion(float a, float b)
	{
		// �ų�   C = A + B - (A * B) / 128
		argb r_a = PhotoshopComput::clump_to_ps_argb(a);
		argb r_b = PhotoshopComput::clump_to_ps_argb(b);
		argb r_1 = 0;
		r_1 = r_a + r_b - r_a * r_b / 128;
		if (r_1 > ARGB_LEVER)
			r_1 = ARGB_LEVER;
		if (r_1 < 0)
			r_1 = 0;
		float r = PhotoshopComput::convert_from_argb(r_1);
		return r;
	}

	// ��ɫ���ģʽ��
	std::vector <float> PhotoshopComput::darker_color(std::vector <float> a, std::vector <float> b)
	{
		// ��ɫ   Br+Bg+Bb >= Ar+Ag+Ab �� C=A
		argb r_a_r = PhotoshopComput::clump_to_ps_argb(a[0]);
		argb r_a_g = PhotoshopComput::clump_to_ps_argb(a[1]);
		argb r_a_b = PhotoshopComput::clump_to_ps_argb(a[2]);
		argb r_b_r = PhotoshopComput::clump_to_ps_argb(b[0]);
		argb r_b_g = PhotoshopComput::clump_to_ps_argb(b[1]);
		argb r_b_b = PhotoshopComput::clump_to_ps_argb(b[2]);
		argb r_1_r = 0; argb r_1_g = 0; argb r_1_b = 0;
		argb aline = r_a_r + r_a_g + r_a_b;
		argb bline = r_b_r + r_b_g + r_b_b;
		if (bline >= aline) {
			r_1_r = r_a_r;
			r_1_g = r_a_g;
			r_1_b = r_a_b;
		}
		else {
			r_1_r = r_b_r;
			r_1_g = r_b_g;
			r_1_b = r_b_b;
		}
		if (r_1_r > ARGB_LEVER)
			r_1_r = ARGB_LEVER;
		if (r_1_g > ARGB_LEVER)
			r_1_g = ARGB_LEVER;
		if (r_1_b > ARGB_LEVER)
			r_1_b = ARGB_LEVER;
		if (r_1_r < 0)
			r_1_r = 0;
		if (r_1_g < 0)
			r_1_g = 0;
		if (r_1_b < 0)
			r_1_b = 0;
		float rR = PhotoshopComput::convert_from_argb(r_1_r);
		float rG = PhotoshopComput::convert_from_argb(r_1_g);
		float rB = PhotoshopComput::convert_from_argb(r_1_b);
		std::vector <float> r = { rR, rG, rB };
		return r;
	}

	std::vector <float> PhotoshopComput::lighter_color(std::vector <float> a, std::vector <float> b)
	{
		// ǳɫ  Br+Bg+Bb >= Ar+Ag+Ab �� C=B
		argb r_a_r = PhotoshopComput::clump_to_ps_argb(a[0]);
		argb r_a_g = PhotoshopComput::clump_to_ps_argb(a[1]);
		argb r_a_b = PhotoshopComput::clump_to_ps_argb(a[2]);
		argb r_b_r = PhotoshopComput::clump_to_ps_argb(b[0]);
		argb r_b_g = PhotoshopComput::clump_to_ps_argb(b[1]);
		argb r_b_b = PhotoshopComput::clump_to_ps_argb(b[2]);
		argb r_1_r = 0; argb r_1_g = 0; argb r_1_b = 0;
		argb aline = r_a_r + r_a_g + r_a_b;
		argb bline = r_b_r + r_b_g + r_b_b;
		if (bline >= aline) {
			r_1_r = r_b_r;
			r_1_g = r_b_g;
			r_1_b = r_b_b;
		}
		else {
			r_1_r = r_a_r;
			r_1_g = r_a_g;
			r_1_b = r_a_b;
		}
		if (r_1_r > ARGB_LEVER)
			r_1_r = ARGB_LEVER;
		if (r_1_g > ARGB_LEVER)
			r_1_g = ARGB_LEVER;
		if (r_1_b > ARGB_LEVER)
			r_1_b = ARGB_LEVER;
		if (r_1_r < 0)
			r_1_r = 0;
		if (r_1_g < 0)
			r_1_g = 0;
		if (r_1_b < 0)
			r_1_b = 0;
		float rR = PhotoshopComput::convert_from_argb(r_1_r);
		float rG = PhotoshopComput::convert_from_argb(r_1_g);
		float rB = PhotoshopComput::convert_from_argb(r_1_b);
		std::vector <float> r = { rR, rG, rB };
		return r;
	}

	std::vector <float> PhotoshopComput::hue(std::vector <float> a, std::vector <float> b)
	{
		// ɫ��
		argb r_a_r = PhotoshopComput::clump_to_ps_argb(a[0]);
		argb r_a_g = PhotoshopComput::clump_to_ps_argb(a[1]);
		argb r_a_b = PhotoshopComput::clump_to_ps_argb(a[2]);
		argb r_b_r = PhotoshopComput::clump_to_ps_argb(b[0]);
		argb r_b_g = PhotoshopComput::clump_to_ps_argb(b[1]);
		argb r_b_b = PhotoshopComput::clump_to_ps_argb(b[2]);
		argb r_1_r = 0; argb r_1_g = 0; argb r_1_b = 0;
		r_1_r = 0.5f;
		if (r_1_r > ARGB_LEVER)
			r_1_r = ARGB_LEVER;
		if (r_1_g > ARGB_LEVER)
			r_1_g = ARGB_LEVER;
		if (r_1_b > ARGB_LEVER)
			r_1_b = ARGB_LEVER;
		if (r_1_r < 0)
			r_1_r = 0;
		if (r_1_g < 0)
			r_1_g = 0;
		if (r_1_b < 0)
			r_1_b = 0;
		float rR = PhotoshopComput::convert_from_argb(r_1_r);
		float rG = PhotoshopComput::convert_from_argb(r_1_g);
		float rB = PhotoshopComput::convert_from_argb(r_1_b);
		std::vector <float> r = { rR, rG, rB };
		return r;
	}
}

static const char* const HELP = "Photoshop layers merge. by wuxiaomeng.";

class PhotoshopMerge : public PixelIop
{
	int layer_index;
	static const char* const enumLayerNames[];
public:
	void in_channels(int input, ChannelSet& mask) const override;
	PhotoshopMerge(Node* node) : PixelIop(node)
	{
		inputs(2);
		layer_index = 0;
	}
	bool pass_transform() const override { return true; }
	void pixel_engine(const Row &in, int y, int x, int r, ChannelMask, Row & out) override;
	void knobs(Knob_Callback) override;
	static const Iop::Description d;
	const char* Class() const override { return d.name; }
	const char* node_help() const override { return HELP; }
	void _validate(bool) override;
	void _request(int x, int y, int r, int t, ChannelMask channels, int count);
};

/*
����      Normal
# �ܽ�     Dissolve
-------------------------
�䰵       Darken
��Ƭ����   Multiply
��ɫ����   ColorBurn
���Լ���    LinearBurn
��ɫ       DarkerColor
-----------------------------
����      Lighten
��ɫ       Screen
��ɫ����    ColorDodge
���Լ���    LinearDodge
ǳɫ       LighterColor
----------------------------
����      Overlay
���     SoftLight
ǿ��    HardLight
����    VividLight
���Թ�   LinearLight
���    PinLight
ʵɫ���  HardMix
----------------------------
��ֵ     Diference
�ų�     Exclusion
---------------------------
ɫ��     Hue
# ���Ͷ�    Saturation
# ��ɫ      Color
# ����      Luminosity
*/

const char* const PhotoshopMerge::enumLayerNames[] = { "normal", "darken", "multiply",
"colorBurn", "linearBurn", "darkerColor", "lighten", "screen", "colorDodge", "linearDodge", "lighterColor",
"overlay", "softLight", "hardLight", "vividLight", "linearLight", "pinLight", "hardMix",
"diference", "exclusion", "hue", NULL};

void PhotoshopMerge::_validate(bool for_real)
{
	input0().validate(for_real);
	input1().validate(for_real);
	copy_info();
	merge_info(1);
	set_out_channels(Mask_All);
	PixelIop::_validate(for_real);
}

void PhotoshopMerge::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
	// request from input 0 and input 1
	input0().request(x, y, r, t, channels, count);
	input1().request(x, y, r, t, channels, count);
}

void PhotoshopMerge::in_channels(int input, ChannelSet& mask) const
{
	// mask is unchanged
}

void PhotoshopMerge::pixel_engine(const Row& in, int y, int x, int r,
	ChannelMask channels, Row& out)
{
	// input 0 row
	Row inA(x, r);
	input0().get(y, x, r, channels, inA);

	// input 1 row
	Row inB(x, r);
	input1().get(y, x, r, channels, inB);

	float (*func)(float, float);
	std::vector <float> (*funcColor)(std::vector <float>, std::vector <float>);
	func = &photoshopMergeTool::PhotoshopComput::normal;
	funcColor = &photoshopMergeTool::PhotoshopComput::hue;
	photoshopMergeTool::PsMode mode = photoshopMergeTool::asValueBlend;

	switch (layer_index)
	{
	case photoshopMergeTool::Normal:
		func = &photoshopMergeTool::PhotoshopComput::normal;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Darken:
		func = &photoshopMergeTool::PhotoshopComput::darken;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Multiply:
		func = &photoshopMergeTool::PhotoshopComput::multiply;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::ColorBurn:
		func = &photoshopMergeTool::PhotoshopComput::color_burn;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::LinearBurn:
		func = &photoshopMergeTool::PhotoshopComput::linear_burn;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::DarkerColor:
		funcColor = &photoshopMergeTool::PhotoshopComput::darker_color;
		mode = photoshopMergeTool::asColorBlend;
		break;
	case photoshopMergeTool::Lighten:
		func = &photoshopMergeTool::PhotoshopComput::lighten;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Screen:
		func = &photoshopMergeTool::PhotoshopComput::screen;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::ColorDodge:
		func = &photoshopMergeTool::PhotoshopComput::color_dodge;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::LinearDodge:
		func = &photoshopMergeTool::PhotoshopComput::linear_dodge;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::LighterColor:
		funcColor = &photoshopMergeTool::PhotoshopComput::lighter_color;
		mode = photoshopMergeTool::asColorBlend;
		break;
	case photoshopMergeTool::Overlay:
		func = &photoshopMergeTool::PhotoshopComput::overlay;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::SoftLight:
		func = &photoshopMergeTool::PhotoshopComput::soft_light;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::HardLight:
		func = &photoshopMergeTool::PhotoshopComput::hard_light;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::VividLight:
		func = &photoshopMergeTool::PhotoshopComput::vivid_light;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::LinearLight:
		func = &photoshopMergeTool::PhotoshopComput::linear_light;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::PinLight:
		func = &photoshopMergeTool::PhotoshopComput::pin_light;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::HardMix:
		func = &photoshopMergeTool::PhotoshopComput::hard_mix;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Diference:
		func = &photoshopMergeTool::PhotoshopComput::diference;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Exclusion:
		func = &photoshopMergeTool::PhotoshopComput::exclusion;
		mode = photoshopMergeTool::asValueBlend;
		break;
	case photoshopMergeTool::Hue:
		funcColor = &photoshopMergeTool::PhotoshopComput::hue;
		mode = photoshopMergeTool::asColorBlend;
		break;
	default:
		func = &photoshopMergeTool::PhotoshopComput::normal;
		mode = photoshopMergeTool::asValueBlend;
	}

	if (mode == photoshopMergeTool::asValueBlend) {
		foreach(z, channels) {
			const float* inptr = inA[z] + x;
			const float* inptrA = inB[z] + x;
			const float* END = inptr + (r - x);
			float* outptr = out.writable(z) + x;
			while (inptr < END)
				*outptr++ = (*func)((*inptr++), (*inptrA++));
		}
	}

	if (mode == photoshopMergeTool::asColorBlend) {
		if (channels.size() >= 3) {
			std::vector <Channel> uchannels;
			int ci = 1;
			foreach(z, channels) {
				uchannels.push_back(z);
			}
			const float* inptrR = inA[uchannels[0]] + x;
			const float* inptrG = inA[uchannels[1]] + x;
			const float* inptrB = inA[uchannels[2]] + x;
			const float* inptrAR = inB[uchannels[0]] + x;
			const float* inptrAG = inB[uchannels[1]] + x;
			const float* inptrAB = inB[uchannels[2]] + x;
			const float* ENDR = inptrR + (r - x);
			float* outptr_r = out.writable(uchannels[0]) + x;
			float* outptr_g = out.writable(uchannels[1]) + x;
			float* outptr_b = out.writable(uchannels[2]) + x;

			while (inptrR < ENDR) {
				std::vector <float> inD = { *inptrR, *inptrG, *inptrB };
				std::vector <float> inDA = { *inptrAR, *inptrAG, *inptrAB };
				std::vector <float> result = (*funcColor)(inD, inDA);
				*outptr_r++ = result[0];
				inptrR++;
				inptrAR++;
				*outptr_g++ = result[1];
				inptrG++;
				inptrAG++;
				*outptr_b++ = result[2]; 
				inptrB++;
				inptrAB++;
			}
		}
	}
}

void PhotoshopMerge::knobs(Knob_Callback f)
{
	CascadingEnumeration_knob(f, &layer_index, &enumLayerNames[0], "blendMode");
}

#include "DDImage/NukeWrapper.h"

static Iop* build(Node* node) { return new NukeWrapper(new PhotoshopMerge(node)); }

const Iop::Description PhotoshopMerge::d("PhotoshopMerge", "PhotoshopMerge", build);

