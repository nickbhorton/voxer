#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

bool constexpr debug_print = false;

// unused because I am lazy lowkey
unsigned int default_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff,
    0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff, 0xff6699ff, 0xff3399ff,
    0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff,
    0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff, 0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff,
    0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc,
    0xff99cccc, 0xff66cccc, 0xff33cccc, 0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc,
    0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc,
    0xff9933cc, 0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
    0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99, 0xffcccc99,
    0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999,
    0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699, 0xff006699, 0xffff3399, 0xffcc3399,
    0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099,
    0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66, 0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66,
    0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966,
    0xff009966, 0xffff6666, 0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366,
    0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33,
    0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933, 0xff669933, 0xff339933,
    0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333,
    0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033, 0xffcc0033, 0xff990033, 0xff660033, 0xff330033,
    0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00,
    0xff99cc00, 0xff66cc00, 0xff33cc00, 0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900,
    0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300,
    0xff993300, 0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
    0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044, 0xff000022,
    0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400,
    0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000, 0xff880000, 0xff770000, 0xff550000,
    0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777,
    0xff555555, 0xff444444, 0xff222222, 0xff111111
};

struct Chunk {
    Chunk() : chunk_id{}, chunk_content_count{}, child_content_count{} {}
    Chunk(const Chunk&) = default;
    Chunk(Chunk&&) = default;
    Chunk& operator=(const Chunk&) = default;
    Chunk& operator=(Chunk&&) = default;

    std::string chunk_id;
    int32_t chunk_content_count;
    int32_t child_content_count;
};

Chunk read_chunk(std::ifstream& file_stream)
{
    Chunk chunk{};
    char chunk_id[4];
    memcpy(chunk_id, "NONE", 4);
    file_stream.read(chunk_id, 4);
    chunk.chunk_id = std::string(chunk_id);
    file_stream.read(reinterpret_cast<char*>(&chunk.chunk_content_count), sizeof(chunk.chunk_content_count));

    if (debug_print) {
        std::cout << chunk.chunk_id << ": num bytes of chunk content: " << chunk.chunk_content_count << "\n";
    }
    file_stream.read(reinterpret_cast<char*>(&chunk.child_content_count), sizeof(chunk.child_content_count));
    if (debug_print) {
        std::cout << chunk.chunk_id << ": num bytes of children chunks: " << chunk.child_content_count << "\n";
    }
    return chunk;
}

std::tuple<std::vector<std::array<uint8_t, 4>>, std::array<std::array<uint8_t, 4>, 256>>
parse_vox_file_2(std::string const& file_name)
{
    std::ifstream file_stream{file_name, std::ios_base::binary | std::ios_base::in};

    // prefile
    {
        char magic_prefix[4];
        file_stream.read(magic_prefix, 4);
        assert(strncmp(magic_prefix, "VOX ", 4) == 0);
        int32_t version{};
        file_stream.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (debug_print) {
            std::cout << "vox version: " << version << "\n";
        }
    }

    std::vector<std::array<uint8_t, 4>> voxels{};
    std::array<std::array<uint8_t, 4>, 256> palette{};
    bool found_palette{false};
    while (!file_stream.eof()) {
        Chunk c = read_chunk(file_stream);
        if (c.chunk_id == "XYZI") {
            int32_t number_voxels{};
            file_stream.read(reinterpret_cast<char*>(&number_voxels), sizeof(number_voxels));
            for (int32_t i = 0; i < number_voxels; i++) {
                std::array<uint8_t, 4> voxel;
                for (size_t j = 0; j < 4; j++) {
                    file_stream.read(reinterpret_cast<char*>(&voxel[j]), sizeof(uint8_t));
                }
                voxels.push_back(voxel);
            }
        } else if (c.chunk_id == "RGBA") {
            for (int i = 0; i < 256; i++) {
                for (int j = 0; j < 4; j++) {
                    file_stream.read(reinterpret_cast<char*>(&palette[i][j]), sizeof(palette[i][j]));
                }
            }
            found_palette = true;
        } else {
            char buffer;
            for (int i = 0; i < c.chunk_content_count; i++) {
                file_stream.read(&buffer, 1);
            }
        }
    }
    file_stream.close();

    std::cout << voxels.size() << " voxels parsed\n";
    if (!found_palette) {
        for (size_t c = 0; c < 256; c++) {
            std::array<uint8_t, 4> ucolor{};
            for (int i = 0; i < 4; i++) {
                ucolor[i] = ((uint8_t*)&default_palette[c])[i];
            }
            palette[c] = ucolor;
        }
    }
    return {voxels, palette};
}

std::tuple<std::vector<std::array<uint8_t, 4>>, std::array<std::array<uint8_t, 4>, 256>>
parse_vox_file(std::string const& file_name)
{
    std::ifstream file_stream{file_name, std::ios_base::binary | std::ios_base::in};
    if (!file_stream.is_open()) {
        assert(1 == 0);
    }
    if (debug_print) {
        std::cout << "parsing " << file_name << "\n";
    }

    char chunk_id[4];

    // prefile
    file_stream.read(chunk_id, 4);
    assert(strncmp(chunk_id, "VOX ", 4) == 0);
    int32_t version{};
    file_stream.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (debug_print) {
        std::cout << "vox version: " << version << "\n";
    }

    // MAIN chunk
    Chunk main = read_chunk(file_stream);
    assert(main.chunk_id == "MAIN");
    assert(main.chunk_content_count == 0);

    // PACK chunk
    int packed = 1;
    Chunk size_chunk = read_chunk(file_stream);
    if (size_chunk.chunk_id == "PACK") {
        file_stream.read(reinterpret_cast<char*>(&packed), sizeof(packed));
        if (debug_print) {
            std::cout << "PACK: " << packed << "\n";
        }
        Chunk size_chunk = read_chunk(file_stream);
        assert(size_chunk.chunk_id == "SIZE");
    } else {
        assert(size_chunk.chunk_id == "SIZE");
    }

    std::vector<std::array<uint8_t, 4>> voxels{};
    while (size_chunk.chunk_id == "SIZE" && !file_stream.eof()) {
        std::array<int32_t, 3> size_data{};
        for (size_t i = 0; i < 3; i++) {
            file_stream.read(reinterpret_cast<char*>(&size_data[i]), sizeof(size_data[i]));
        }
        if (debug_print) {
            std::cout << "SIZE data: " << size_data[0] << ", " << size_data[1] << ", " << size_data[2] << "\n";
        }

        Chunk xyzi = read_chunk(file_stream);
        assert(xyzi.chunk_id == "XYZI");

        int32_t number_voxels{};
        file_stream.read(reinterpret_cast<char*>(&number_voxels), sizeof(number_voxels));
        if (debug_print) {
            std::cout << "XYZI voxels: " << number_voxels << "\n";
        } else {
            std::cout << "voxels count " << number_voxels << "\n";
        }
        for (int32_t i = 0; i < number_voxels; i++) {
            std::array<uint8_t, 4> voxel;
            for (size_t j = 0; j < 4; j++) {
                file_stream.read(reinterpret_cast<char*>(&voxel[j]), sizeof(uint8_t));
            }
            voxels.push_back(voxel);
        }
        std::cout << voxels.size() << "\n";
        size_chunk = read_chunk(file_stream);
    }

    std::array<std::array<uint8_t, 4>, 256> palette{};
    if (file_stream.eof()) {
        if (debug_print) {
            std::cout << "PALETTE: use default palette\n";
        }
        for (size_t c = 0; c < 256; c++) {
            std::array<uint8_t, 4> ucolor{};
            for (int i = 0; i < 4; i++) {
                ucolor[i] = ((uint8_t*)&default_palette[c])[i];
            }
            palette[c] = ucolor;
        }
    } else {
        Chunk rgba = size_chunk;
        while (rgba.chunk_id != "RGBA" && !file_stream.eof()) {
            for (int i = 0; i < rgba.chunk_content_count; i++) {
                char n;
                file_stream.read(reinterpret_cast<char*>(&n), sizeof(n));
            }
            rgba = read_chunk(file_stream);
        }
        assert(rgba.chunk_id == "RGBA");
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 4; j++) {
                file_stream.read(reinterpret_cast<char*>(&palette[i][j]), sizeof(palette[i][j]));
            }
        }
    }
    file_stream.close();
    return {voxels, palette};
}

/*
int main(int argc, char** argv)
{
    if (argc < 2) {
        exit(1);
    }
    std::vector<std::string> files_to_parse{};
    for (int i = 1; i < argc; i++) {
        files_to_parse.push_back(argv[i]);
    }
    for (auto const& file_name : files_to_parse) {
        parse_vox_file(file_name);
    }
}
*/
