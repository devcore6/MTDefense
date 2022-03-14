export module renderer.color_format;

export enum color_format: unsigned char {
    FORMAT_RGB8 = 0,
    FORMAT_RGBA8,
    FORMAT_ARGB8,
    FORMAT_BGR8,
    FORMAT_BGRA8,
    FORMAT_ABGR8,
    FORMAT_MAX
};
