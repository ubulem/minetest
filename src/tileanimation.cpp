/*
Minetest
Copyright (C) 2016 sfan5 <sfan5@live.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "tileanimation.h"
#include "util/serialize.h"

void TileAnimationParams::serialize(std::ostream &os, u16 protocol_version) const
{
	if(protocol_version < 29 /* TODO bump */) {
		if (type == TAT_VERTICAL_FRAMES) {
			writeU8(os, type);
			writeU16(os, vertical_frames.aspect_w);
			writeU16(os, vertical_frames.aspect_h);
			writeF1000(os, vertical_frames.length);
		} else {
			writeU8(os, TAT_NONE);
			writeU16(os, 1);
			writeU16(os, 1);
			writeF1000(os, 1.0);
		}
		return;
	}

	writeU8(os, type);
	if (type == TAT_VERTICAL_FRAMES) {
		writeU16(os, vertical_frames.aspect_w);
		writeU16(os, vertical_frames.aspect_h);
		writeF1000(os, vertical_frames.length);
	}
}

void TileAnimationParams::deSerialize(std::istream &is, u16 protocol_version)
{
	type = (TileAnimationType) readU8(is);
	if(protocol_version < 29 /* TODO bump */) {
		vertical_frames.aspect_w = readU16(is);
		vertical_frames.aspect_h = readU16(is);
		vertical_frames.length = readF1000(is);
		return;
	}

	if(type == TAT_VERTICAL_FRAMES) {
		vertical_frames.aspect_w = readU16(is);
		vertical_frames.aspect_h = readU16(is);
		vertical_frames.length = readF1000(is);
	}
}

void TileAnimationParams::determineParams(v2u32 texture_size, int *frame_count, int *frame_length_ms) const
{
	if (type == TAT_NONE) {
		*frame_count = 1;
		*frame_length_ms = 1000;
		return;
	}
	int frame_height = (float)texture_size.X /
			(float)vertical_frames.aspect_w *
			(float)vertical_frames.aspect_h;
	if (frame_count)
		*frame_count = texture_size.Y / frame_height;
	if (frame_length_ms)
		*frame_length_ms = 1000.0 * vertical_frames.length / (texture_size.Y / frame_height);
}

void TileAnimationParams::getTextureModifer(std::ostream &os, v2u32 texture_size, int frame) const
{
	if (type == TAT_NONE)
		return;
	int frame_count;
	determineParams(texture_size, &frame_count, NULL);
	os << "^[verticalframe:" << frame_count << ":" << frame;
}
