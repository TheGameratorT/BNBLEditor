#pragma once
#include <QVector>

class BNBL
{
public:
	struct Rectangle
	{
		quint16 xpos;
		quint16 ypos;
		quint8 width;
		quint8 height;
	};

	enum LoadResult
	{
		SUCCESS,
		FILE_INACCESSIBLE,
		MAGIC_MISMATCH
	};

	QVector<Rectangle> rectangles;

	LoadResult loadFromFile(const QString& path);
	bool saveToFile(const QString& path);
};

