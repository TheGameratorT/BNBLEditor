#include "BNBL.h"

#include <QFile>
#include <QDataStream>

//Decompiled from NSMB
int GetPosForRectangle(quint16 pos_field, int width_or_height)
{
	unsigned int anchor = pos_field << 18 >> 30;
	int position = pos_field << 20 >> 20;
	if (anchor == 1)                            // center
	{
		position -= (width_or_height + 1) / 2;
	}
	else if (anchor == 2)                       // down/right
	{
		position -= width_or_height;
	}
	return position;
}

BNBL::LoadResult BNBL::loadFromFile(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		in.setByteOrder(QDataStream::LittleEndian);

		quint8 magic[4] = { 'J', 'N', 'B', 'L' };
		for (int i = 0; i < 4; i++)
		{
			quint8 magic_chr;
			in >> magic_chr;
			if (magic_chr != magic[i])
			{
				file.close();
				return MAGIC_MISMATCH;
			}
		}

		quint16 padding;
		in >> padding;

		quint16 rectCount;
		in >> rectCount;

		this->rectangles.clear();
		if (rectCount > 0)
		{
			for (int i = 0; i < rectCount; i++)
			{
				Rectangle rect;
				in >> rect.xpos;
				in >> rect.ypos;
				in >> rect.width;
				in >> rect.height;
				rect.xpos = GetPosForRectangle(rect.xpos, rect.width);
				rect.ypos = GetPosForRectangle(rect.ypos, rect.height);
				this->rectangles.append(rect);
			}
		}

		file.close();
		return SUCCESS;
	}
	else
	{
		return FILE_INACCESSIBLE;
	}
}

bool BNBL::saveToFile(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream out(&file);
		out.setByteOrder(QDataStream::LittleEndian);

		for(quint8 chr : { 'J', 'N', 'B', 'L' })
			out << chr;
		out << (quint16)0;
		out << (quint16)this->rectangles.count();
		for (Rectangle rect : this->rectangles)
		{
			out << rect.xpos;
			out << rect.ypos;
			out << rect.width;
			out << rect.height;
		}

		file.close();
		return true;
	}
	else
	{
		return false;
	}
}