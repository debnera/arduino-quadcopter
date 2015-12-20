/*
Created:	9/7/2015
Author:		Anton
*/

#ifndef _VECTOR4_h
#define _VECTOR4_h


class Vector4
{

public:
	int x1;
	int x2;
	int x3;
	int x4;

	Vector4(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0)
	{
		x1 = value1;
		x2 = value2;
		x3 = value3;
		x4 = value4;
	};

	void setValues(int value1 = 0, int value2 = 0, int value3 = 0, int value4 = 0)
	{
		x1 = value1;
		x2 = value2;
		x3 = value3;
		x4 = value4;
	};

	Vector4 operator+(Vector4 another_vector)
	{
		Vector4 new_vec = Vector4();
		new_vec.x1 = this->x1 + another_vector.x1;
		new_vec.x2 = this->x2 + another_vector.x2;
		new_vec.x3 = this->x3 + another_vector.x3;
		new_vec.x4 = this->x4 + another_vector.x4;
		return new_vec;
	};

	Vector4 operator-(Vector4 another_vector)
	{
		Vector4 new_vec = Vector4();
		new_vec.x1 = this->x1 - another_vector.x1;
		new_vec.x2 = this->x2 - another_vector.x2;
		new_vec.x3 = this->x3 - another_vector.x3;
		new_vec.x4 = this->x4 - another_vector.x4;
		return new_vec;
	};

	Vector4 operator+(int value)
	{
		return Vector4(x1 + value, x2 + value, x3 + value, x4 + value);
	};
};


#endif
