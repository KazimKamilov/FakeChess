#ifndef _FAKE_CHESS_FIELD_COORDS_HPP_
#define _FAKE_CHESS_FIELD_COORDS_HPP_


struct FieldCoords final
{
	explicit FieldCoords(const size_t coord_x, const size_t coord_y) :
		x {static_cast<int>(coord_x)},
		y {static_cast<int>(coord_y)}
	{
	}

	int x = -1;
	int y = -1;
};

#endif
