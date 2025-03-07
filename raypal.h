#ifndef RAYPAL_H
#define RAYPAL_H

#include "raylib.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

void LoadPAL(const char *fileName, Color **paletteColors, unsigned int *paletteSize)
{
	FILE *file = fopen(fileName, "r");

	if (file != NULL)
	{
		char line[16];
		unsigned int lineCounter = 0;
		unsigned char charCounter, currentChar, colorCount = 0;

		while (fgets(line, 16, file))
		{
			line[strcspn(line, "\n")] = 0; // Replace newline character with nul terminator

			// 1st line should contain "JASC-PAL"
			if (lineCounter == 0)
			{
				if (strcmp(line, "JASC-PAL") != 0)
				{
					TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: \"%s\" (line %i) doesn't match \"JASC-PAL\"", fileName, line, lineCounter + 1);
					fclose(file);
					return;
				}
			}

			// 2nd line should contain the version number, usually "0100"
			else if (lineCounter == 1)
			{
				charCounter = 0;
				currentChar = line[charCounter];

				while (currentChar != 0)
				{
					if (!isdigit(line[charCounter]))
					{
						TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: \"%s\" (line %i) is not a valid version number", fileName, line, lineCounter + 1);
						fclose(file);
						return;
					}
					currentChar = line[++charCounter];
				}
			}

			// 3rd line should contain the number of colors in the palette
			else if (lineCounter == 2)
			{
				charCounter = 0;
				currentChar = line[charCounter];

				while (currentChar != 0)
				{
					if (!isdigit(line[charCounter]))
					{
						TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: \"%s\" (line %i) is not a number", fileName, line, lineCounter + 1);
						fclose(file);
						return;
					}
					colorCount *= 10;
					colorCount += currentChar - '0';
					currentChar = line[++charCounter];
				}

				*paletteColors = (Color *)RL_MALLOC(colorCount*sizeof(Color));
				if (*paletteColors == NULL)
				{
					TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: not enough memory for palette colors", fileName);
					fclose(file);
					return;
				}
			}

			// All lines after the 3rd should contain RGBA values.
			// If there are less than 4 values written, the rest are filled with 255.
			// For example, "72 59 200" becomes "72 59 200 255", giving it an opaque alpha by default.
			else
			{
				
				// We only add as many colors as the file indicated
				if (lineCounter - 3 >= colorCount) break;

				charCounter = 0;
				currentChar = line[charCounter];
				unsigned char currentAttribute = 0, lineColor[4] = { 0, 255, 255, 255 };

				if (currentChar == 0)
				{
					TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: color data is empty", fileName);
					fclose(file);
					return;
				}

				while(currentChar != 0)
				{
					if (currentChar == ' ') 
					{
						currentAttribute++;
						lineColor[currentAttribute] = 0;
					}
					else
					{
						if (!isdigit(currentChar))
						{
							TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: \"%s\" (line %i) is not a valid color", fileName, line, lineCounter + 1);
							fclose(file);
							RL_FREE(*paletteColors);
							return;
						}
						lineColor[currentAttribute] *= 10;
						lineColor[currentAttribute] += currentChar - '0';
					}
					currentChar = line[++charCounter];
				}

				(*paletteColors)[lineCounter - 3].r = lineColor[0];
				(*paletteColors)[lineCounter - 3].g = lineColor[1];
				(*paletteColors)[lineCounter - 3].b = lineColor[2];
				(*paletteColors)[lineCounter - 3].a = lineColor[3];
			}

			lineCounter++;
		}

		*paletteSize = colorCount;
		TraceLog(LOG_INFO, "PALETTE: [%s] Palette loaded successfully (%i colors)", fileName, colorCount);
		fclose(file);
	}
	else TraceLog(LOG_WARNING, "[%s] Could not load palette: empty file", fileName);
}

void LoadPalette(const char *fileName, Color **colors, unsigned int *size)
{
	if (IsFileExtension(fileName, ".pal")) LoadPAL(fileName, colors, size);
	else TraceLog(LOG_WARNING, "PALETTE: [%s] Could not load palette: extension not supported", fileName);
}

#endif