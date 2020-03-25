textures/13agony/support1
{
	qer_editorimage textures/base_support/support1shiny.tga
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_support/support1shiny.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/glue_trans
{
	qer_editorimage textures/13agony/glue.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/glue.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/metal3_3_trans
{
	qer_editorimage textures/base_support/metal3_3.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_support/metal3_3.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/concrete_dark_trans
{
	qer_editorimage textures/13agony/concrete_dark.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/concrete_dark.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/rusty_pewter_big_trans
{
	qer_editorimage textures/base_trim/rusty_pewter_big.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/rusty_pewter_big.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/concrete_trans
{
	qer_editorimage textures/13agony/concrete.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/concrete.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/concrete_dvt_trans
{
	qer_editorimage textures/13agony/concrete_dvt.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/concrete_dvt.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/manhole_trans
{
	qer_editorimage textures/base_floor/manhole.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_floor/manhole.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/metalsupport4j_dark_trans
{
	qer_editorimage textures/gothic_trim/metalsupport4j.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/gothic_trim/metalsupport4j.tga
		blendfunc filter
		rgbGen const ( 0.7 0.7 0.7 )
	}
}

textures/13agony/xian_dm3padwall_x_trans
{
	qer_editorimage textures/base_trim/xian_dm3padwall_x.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/xian_dm3padwall_x.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/xian_dm3padwall_x2_decal
{
	qer_editorimage textures/base_trim/xian_dm3padwall_x2.tga
	polygonoffset
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/xian_dm3padwall_x2.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/xian_dm3padwall
{
	qer_editorimage textures/sfx/xian_dm3padwall.tga
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/sfx/xian_dm3padwall.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/xian_dm3padwall_fx
{
	qer_editorimage textures/sfx/xian_dm3padwall.tga
	{
		map textures/sfx/xian_dm3padwall.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/xian_dm3padwall_glow.tga
		blendfunc add
		rgbGen wave sin 0 1 0 1 
		tcMod scale 1 0.1
		tcMod scroll 0 2
	}
}

textures/13agony/xian_dm3padwall_glow
{
	qer_editorimage textures/13agony/xian_dm3padwall_glow.tga
	{
		map textures/sfx/xian_dm3padwall.tga
		rgbGen identity
		tcMod scale 1 2
	}
	{
		map textures/13agony/xian_dm3padwall_glow.tga
		blendfunc add
		rgbGen const ( 0.7 0.6 0.4 )
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

textures/13agony/xian_dm3padwall_glow_trans
{
	qer_editorimage textures/13agony/xian_dm3padwall_glow.tga
	surfaceparm nonsolid
	{
		map textures/sfx/xian_dm3padwall.tga
		rgbGen identity
		tcMod scale 1 2
	}
	{
		map textures/13agony/xian_dm3padwall_glow.tga
		blendfunc add
		rgbGen const ( 0.7 0.6 0.4 )
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

textures/13agony/melty_pewter_filter
{
	qer_editorimage textures/base_trim/melty_pewter.tga
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap
	}
	{
		map textures/sfx/beam.tga
		blendfunc gl_one gl_src_color
		rgbGen identity
		tcMod scale 4 0.6
	}
	{
		map textures/base_trim/melty_pewter.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/demonltblackfinal_trans
{
	qer_editorimage textures/sfx/demonltblackfinal.tga
	q3map_lightimage textures/sfx/demonltblackfinal_glow2.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 100
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/sfx/demonltblackfinal.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/sfx/demonltblackfinal_glow2.tga
		blendfunc add
		rgbGen wave sin 0.9 0.1 0 5 
	}
}

textures/13agony/pewter_trans
{
	qer_editorimage textures/base_trim/pewter.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/pewter.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/spiderbit_trans
{
	qer_editorimage textures/base_trim/spiderbit.tga
	q3map_lightimage textures/base_trim/spiderbit_fx.tga
	surfaceparm nonsolid
	q3map_surfacelight 100
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/spiderbit.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_trim/spiderbit_fx.tga
		blendfunc add
		rgbGen wave triangle 0.1 0.5 0 7 
	}
}

textures/13agony/spiderbit3_trans
{
	qer_editorimage textures/base_trim/spiderbit3.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/spiderbit3.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/spiderbit4_trans
{
	qer_editorimage textures/base_trim/spiderbit4.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/spiderbit4.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/xspiderbit4_trans
{
	qer_editorimage textures/base_trim/xspiderbit4.tga
	surfaceparm nonsolid
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_trim/xspiderbit4.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/protolight2k_trans
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 2000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/proto_light.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightmap.tga
		blendfunc add
	}
	{
		map textures/base_light/proto_light2.tga
		blendfunc add
		rgbGen wave noise 0 1 0 0.1 
	}
}

textures/13agony/ceil_yellow4k_trans
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 4000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
		rgbGen identity
	}
}

textures/13agony/ceil_yellow_trans
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 10000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
		rgbGen identity
	}
}

textures/13agony/ceil_red_trans
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 10000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
		rgbGen identity
	}
}

textures/13agony/ceil_green_trans
{
	qer_editorimage textures/base_light/ceil1_34.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 10000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
		rgbGen identity
	}
}

textures/13agony/ceil_yellow_dark
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 1000
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/bounce_concrete_spec
{
	qer_editorimage textures/sfx/bounce_concrete.tga
	q3map_lightimage textures/sfx/jumppadsmall.tga
	surfaceparm nodamage
	surfaceparm trans
	q3map_surfacelight 200
	{
		map textures/sfx/bounce_concrete.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/sfx/bouncepad01b_layer1.tga
		blendfunc add
		rgbGen wave sin 0.5 0.5 0 1.5 
	}
	{
		clampmap textures/sfx/jumppadsmall.tga
		blendfunc add
		rgbGen wave square 0.5 0.5 0.25 1.5 
		tcMod stretch sin 1.2 0.8 0 1.5 
	}
}

textures/13agony/x_support2
{
	qer_editorimage textures/base_support/x_support2.tga
	surfaceparm alphashadow
	surfaceparm nomarks
	surfaceparm trans
	nopicmip
	{
		map textures/base_support/x_support2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/13agony/x_support2_back
{
	qer_editorimage textures/base_support/x_support2.tga
	surfaceparm alphashadow
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	nopicmip
	{
		map textures/base_support/x_support2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/13agony/x_crate
{
	qer_editorimage textures/base_trim/panel01.tga
	nopicmip
	{
		map textures/base_trim/panel01.tga
		rgbGen const ( 0.7 0.7 0.7 )
	}
	{
		map textures/base_support/x_support.tga
		blendfunc blend
		rgbGen const ( 0.7 0.7 0.7 )
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

textures/13agony/grate
{
	qer_editorimage textures/base_floor/pjgrate1.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm playerclip
	nopicmip
	{
		map textures/base_floor/pjgrate1.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/13agony/q3abanner_red
{
	{
		map textures/sfx/snow.tga
		rgbGen const ( 0.5 0.5 0.5 )
		tcMod scroll 10 2
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/q3abanner_red.tga
		blendfunc add
		rgbGen wave square 0 0.6 0 0.5 
	}
	{
		map textures/effects/tinfx3.tga
		blendfunc add
		rgbGen const ( 0.2 0.2 0.2 )
		tcGen environment 
	}
}

textures/13agony/decal_glow
{
	surfaceparm nodlight
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	sort 5
	polygonoffset
	{
		map gfx/damage/hole_lg_mrk.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen const ( 0.7 0.75 0.8 )
	}
	{
		clampmap textures/13agony/decal_glow.tga
		blendfunc add
		rgbGen identity
		tcMod stretch square 0.7 0 0 1 
	}
}

textures/13agony/protobanner
{
	qer_editorimage textures/base_wall/protobanner.tga
	surfaceparm alphashadow
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	cull disable
	{
		map textures/base_wall/protobanner.tga
		rgbGen Vertex
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/13agony/rock_phong
{
	qer_editorimage textures/13agony/rock_grey2.tga
	q3map_nonplanar
	q3map_shadeangle 120
	q3map_lightmapMergable
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/13agony/rock_grey2.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/13agony/black
{
	qer_editorimage textures/skies/blacksky.tga
	surfaceparm nodamage
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map gfx/colors/black.tga
		rgbGen identity
	}
}

textures/13agony/metal_clip
{
	qer_editorimage textures/13quake/common_metalclip.tga
	surfaceparm metalsteps
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm playerclip
	qer_trans 0.4
}

textures/13agony/slick_botclip
{
	qer_editorimage textures/common/slick.tga
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm slick
	surfaceparm trans
	surfaceparm botclip
	qer_trans 0.4
}

textures/13agony/cushion_clip
{
	qer_editorimage textures/common/cushion.tga
	surfaceparm nodamage
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm playerclip
	qer_trans 0.4
}

