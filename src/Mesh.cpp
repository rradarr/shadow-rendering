#include "stdafx.hpp"
#include "Mesh.hpp"

//#include "tiny_obj_loader.h"

#include "DXSampleHelper.h"
#include "EngineHelpers.hpp"
#include "BufferMemoryManager.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<DWORD> indices)
{
    CreateMeshVertexAndIndexBuffers(vertices, indices);
}

void Mesh::CreateFromFile(const std::string fileName, std::vector<DirectX::XMFLOAT4>* boundingBox)
{
    std::vector<Vertex> triangleVertices;
    std::vector<DWORD> triangleIndices;
    
    // TODO: handle a bad return.
    bool ret = LoadModelFromFile(fileName, triangleVertices, triangleIndices);

    if(boundingBox != nullptr) {
        *boundingBox = GetBoundingBox(triangleVertices);
    }
    
    CreateMeshVertexAndIndexBuffers(triangleVertices, triangleIndices);
}

void Mesh::CreatePlane(std::vector<Vertex>& vertices, std::vector<DWORD>& indices)
{
    vertices.reserve(4);

    Vertex vert;
    vert.color = DirectX::XMFLOAT4{1.f, 1.f, 1.f, 1.f};
    vert.normal = DirectX::XMFLOAT3{0.f, 1.f, 0.f};

    // Back left vertex.
    vert.position = DirectX::XMFLOAT3{-0.5f, 0.f, -0.5f};
    vert.uvCoordinates = DirectX::XMFLOAT2{0.f, 1.f};
    vertices.push_back(vert);

    // Back right vertex.
    vert.position = DirectX::XMFLOAT3{0.5f, 0.f, -0.5f};
    vert.uvCoordinates = DirectX::XMFLOAT2{1.f, 1.f};
    vertices.push_back(vert);

    // Front left vertex.
    vert.position = DirectX::XMFLOAT3{-0.5f, 0.f, 0.5f};
    vert.uvCoordinates = DirectX::XMFLOAT2{0.f, 0.f};
    vertices.push_back(vert);

    // Front right vertex.
    vert.position = DirectX::XMFLOAT3{0.5f, 0.f, 0.5f};
    vert.uvCoordinates = DirectX::XMFLOAT2{1.f, 0.f};
    vertices.push_back(vert);

    indices = std::vector<DWORD>{0, 2, 3, 3, 1, 0};
}

void Mesh::CreateCube(std::vector<Vertex>& vertices, std::vector<DWORD>& indices, const int resolution)
{
    assert(resolution > 1);

    int vertexRowLength = (resolution - 1) * 4;

    int sideVertexTotal = resolution * (resolution - 1);
    int topBottomVertexTotal = (resolution - 2) * (resolution - 2);
    int vertexTotal = ((sideVertexTotal * 4) + (2 * topBottomVertexTotal));
    vertices.reserve(vertexTotal);

    Vertex vert;
    vert.color = DirectX::XMFLOAT4(1, 1, 1, 1);
    vert.uvCoordinates = { 0.f, 0.f };
    vert.normal = { 0.f, 0.f, 0.f };

    // Generate the cube.

    // Generate cube walls as one long vertex list (a plane that can be folded into the walls of a cube).
    DirectX::XMVECTOR sideFaceNormals[] = {
        DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),   // front
        DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),  // right
        DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),  // back
        DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),   // left
    };
    DirectX::XMVECTOR globalUp = { 0.f, 1.f, 0.f, 0.f };
    // Generate each wall with one less column of vertices at the edge.
    for (int y = 0; y < resolution; y++)
    {
        for (int face = 0; face < 4; face++)
        {
            DirectX::XMVECTOR sideNormal = sideFaceNormals[face];
            DirectX::XMVECTOR yAxis = globalUp;
            DirectX::XMVECTOR xAxis = DirectX::XMVector3Cross(yAxis, sideNormal);

            for (int x = 0; x < resolution - 1; x++)
            {
                float xPercent = (float)x / (resolution - 1);
                float yPercent = (float)y / (resolution - 1);
                float xStage = (xPercent - 0.5f) * 2.0f;
                float yStage = (yPercent - 0.5f) * 2.0f;

                DirectX::XMVECTOR px = DirectX::XMVectorScale(xAxis, xStage);
                DirectX::XMVECTOR py = DirectX::XMVectorScale(yAxis, yStage);
                DirectX::XMVECTOR point = DirectX::XMVectorAdd(sideNormal, px);
                point = DirectX::XMVectorAdd(point, py);
                //point = DirectX::XMVector3Normalize(point);
                DirectX::XMStoreFloat3(&vert.position, point);
                DirectX::XMStoreFloat3(&vert.normal, sideNormal);

                vertices.push_back(vert);
            }
        }
    }

    // Generate cube top and bootom (only inner/non-edge vertices).
    DirectX::XMVECTOR TopBottomSideNormals[] = {
        DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),   // top
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),  // bottom
    };
    DirectX::XMVECTOR globalRight = { 1.f, 0.f, 0.f, 0.f };
    for (int face = 0; face < 2; face++)
    {
        DirectX::XMVECTOR sideNormal = TopBottomSideNormals[face];
        DirectX::XMVECTOR xAxis = DirectX::XMVectorNegate(DirectX::XMVector3Cross(sideNormal, globalRight));
        if (face == 1)
            xAxis = DirectX::XMVectorNegate(xAxis);
        DirectX::XMVECTOR yAxis = DirectX::XMVectorNegate(globalRight);

        // Generate only internal vertices (no top and bottom row, and no left and right column).
        for (int y = 1; y < resolution - 1; y++)
        {
            for (int x = 1; x < resolution - 1; x++)
            {
                float xPercent = (float)x / (resolution - 1);
                float yPercent = (float)y / (resolution - 1);
                float xStage = (xPercent - 0.5f) * 2.0f;
                float yStage = (yPercent - 0.5f) * 2.0f;

                DirectX::XMVECTOR px = DirectX::XMVectorScale(xAxis, xStage);
                DirectX::XMVECTOR py = DirectX::XMVectorScale(yAxis, yStage);
                DirectX::XMVECTOR point = DirectX::XMVectorAdd(sideNormal, px);
                point = DirectX::XMVectorAdd(point, py);
                //point = DirectX::XMVector3Normalize(point);
                DirectX::XMStoreFloat3(&vert.position, point);
                DirectX::XMStoreFloat3(&vert.normal, sideNormal);

                vertices.push_back(vert);
            }
        }
    }

    // Generate the indices.
    int indexTotal = 36 * (resolution - 1) * (resolution - 1); // (6 faces * squares per face * 2 tris per square * 3 indices per tris)
    indices.reserve(indexTotal);

    // Generate indices for sides.
    for (int y = 0; y < resolution - 1; y++) {
        for (int x = 0; x < vertexRowLength - 1; x++) {
            int vertexId = x + y * vertexRowLength;

            indices.push_back(vertexId);
            indices.push_back(vertexId + 1);
            indices.push_back(vertexId + vertexRowLength + 1);

            indices.push_back(vertexId);
            indices.push_back(vertexId + vertexRowLength + 1);
            indices.push_back(vertexId + vertexRowLength);
        }
    }
    // Connect sides at last edge.
    for (int y = 0; y < resolution - 1; y++) {
        int vertexId = (vertexRowLength - 1) + y * vertexRowLength;

        indices.push_back(vertexId);
        indices.push_back(y * vertexRowLength);
        indices.push_back((y + 1) * vertexRowLength);

        indices.push_back(vertexId);
        indices.push_back((y + 1) * vertexRowLength);
        indices.push_back(vertexId + vertexRowLength);
    }

    // Create pseudo sides for top and bottom, for easier triangulation.
    std::vector<int> topSideHelper;
    topSideHelper.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        topSideHelper.push_back(i); // indices of 'top' edge.
    }
    // fill the middle space with vertex indices.
    for (int y = 0; y < resolution - 2; y++) {
        topSideHelper.push_back(4 * (resolution - 1) - 1 - y); // left vertex of this row
        for (int x = 0; x < resolution - 2; x++) {
            topSideHelper.push_back(4 * sideVertexTotal + x + y * (resolution - 2));
        }
        topSideHelper.push_back(resolution + y); // right vertex of this row
    }
    for (int i = 3 * (resolution - 1); i >= 2 * (resolution - 1); i--) {
        topSideHelper.push_back(i); // indices of 'bottom' edge.
    }

    std::vector<int> bottomSideHelper;
    bottomSideHelper.reserve(resolution * resolution);
    for (int i = 0; i < resolution; i++) {
        bottomSideHelper.push_back(i + (resolution - 1) * vertexRowLength); // indices of 'top' edge.
    }
    // fill the middle space with vertex indices.
    for (int y = 0; y < resolution - 2; y++) {
        bottomSideHelper.push_back(4 * (resolution - 1) - 1 - y + (resolution - 1) * vertexRowLength); // left vertex of this row
        for (int x = 0; x < resolution - 2; x++) {
            bottomSideHelper.push_back(4 * sideVertexTotal + topBottomVertexTotal + x + y * (resolution - 2));
        }
        bottomSideHelper.push_back(resolution + y + (resolution - 1) * vertexRowLength); // right vertex of this row
    }
    for (int i = 3 * (resolution - 1) + (resolution - 1) * vertexRowLength; i >= 2 * (resolution - 1) + (resolution - 1) * vertexRowLength; i--) {
        bottomSideHelper.push_back(i); // indices of 'bottom' edge.
    }

    // Generate indices for top and bottom using the heplers.
    for (int y = 0; y < resolution - 1; y++) {
        for (int x = 0; x < resolution - 1; x++) {
            int helperIdPos = x + y * resolution; // Position of the index we want in the hepler vertex.
            indices.push_back(topSideHelper[helperIdPos]);
            indices.push_back(topSideHelper[helperIdPos + resolution + 1]);
            indices.push_back(topSideHelper[helperIdPos + 1]);

            indices.push_back(topSideHelper[helperIdPos + resolution + 1]);
            indices.push_back(topSideHelper[helperIdPos]);
            indices.push_back(topSideHelper[helperIdPos + resolution]);
        }
    }
    for (int y = 0; y < resolution - 1; y++) {
        for (int x = 0; x < resolution - 1; x++) {
            int helperIdPos = x + y * resolution; // Position of the index we want in the hepler vertex.
            indices.push_back(bottomSideHelper[helperIdPos]);
            indices.push_back(bottomSideHelper[helperIdPos + 1]);
            indices.push_back(bottomSideHelper[helperIdPos + resolution + 1]);

            indices.push_back(bottomSideHelper[helperIdPos]);
            indices.push_back(bottomSideHelper[helperIdPos + resolution + 1]);
            indices.push_back(bottomSideHelper[helperIdPos + resolution]);
        }
    }
}

Mesh Mesh::CreateCube(const int resolution)
{
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;

    Mesh::CreateCube(vertices, indices, resolution);

    return Mesh(vertices, indices);
}

void Mesh::CreateSphere(std::vector<Vertex>& vertices, std::vector<DWORD>& indices, const int resolution)
{
    CreateCube(vertices, indices, resolution);
    for (int i = 0; i < vertices.size(); i++) {
        DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&vertices[i].position);
        position = DirectX::XMVector3Normalize(position);
        DirectX::XMStoreFloat3(&vertices[i].position, position);
    }
}

std::vector<DirectX::XMFLOAT4> Mesh::GetBoundingBox(std::vector<Vertex>& meshVertices)
{
    std::vector<DirectX::XMFLOAT4> vertices;
    for(int i = 0; i < meshVertices.size(); i++) {
        DirectX::XMFLOAT3 pos = meshVertices[i].position;
        vertices.push_back(DirectX::XMFLOAT4{pos.x, pos.y, pos.z, 1.f});
    }
    return GetBoundingBox(vertices);
}

std::vector<DirectX::XMFLOAT4> Mesh::GetBoundingBox(std::vector<DirectX::XMFLOAT4>& meshVertices)
{
    std::vector<DirectX::XMFLOAT4> boundingBox;
    boundingBox.reserve(8);
    
    std::vector<DirectX::XMFLOAT4> minMaxPoints = GetMinMaxPoints(meshVertices);

    // Create the bounding vertices.
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[0].x, minMaxPoints[0].y, minMaxPoints[0].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[0].x, minMaxPoints[1].y, minMaxPoints[0].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[0].x, minMaxPoints[0].y, minMaxPoints[1].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[0].x, minMaxPoints[1].y, minMaxPoints[1].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[1].x, minMaxPoints[0].y, minMaxPoints[0].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[1].x, minMaxPoints[1].y, minMaxPoints[0].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[1].x, minMaxPoints[0].y, minMaxPoints[1].z, 1.f});
    boundingBox.push_back(DirectX::XMFLOAT4{minMaxPoints[1].x, minMaxPoints[1].y, minMaxPoints[1].z, 1.f});

    return boundingBox;
}

std::vector<DirectX::XMFLOAT4> Mesh::GetMinMaxPoints(std::vector<DirectX::XMFLOAT4>& points)
{
    DirectX::XMFLOAT4 initialPos = points[0];
    float minX = initialPos.x;
    float maxX = initialPos.x;
    float minY = initialPos.y;
    float maxY = initialPos.y;
    float minZ = initialPos.z;
    float maxZ = initialPos.z;
    float minW = initialPos.w;
    float maxW = initialPos.w;
    for(int i = 1; i < points.size(); i++) {
        DirectX::XMFLOAT4 pos = points[i];
        if(pos.x < minX) {
            minX = pos.x;
        }
        else if (pos.x > maxX) {
            maxX = pos.x;
        }
        if(pos.y < minY) {
            minY = pos.y;
        }
        else if (pos.y > maxY) {
            maxY = pos.y;
        }
        if(pos.z < minZ) {
            minZ = pos.z;
        }
        else if (pos.z > maxZ) {
            maxZ = pos.z;
        }
        if(pos.w < minW) {
            minW = pos.w;
        }
        else if (pos.w > maxW) {
            maxW = pos.w;
        }
    }

    return std::vector<DirectX::XMFLOAT4> {
        {minX, minY, minZ, minW},
        {maxX, maxY, maxZ, maxW}
    };
}

void Mesh::InsertDrawIndexed(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}

void Mesh::InsertBufferBind(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);
}

bool Mesh::LoadModelFromFile(const std::string fileName, std::vector<Vertex>& meshVertices, std::vector<DWORD>& meshIndices)
{
    std::string inputFile = { EngineHelpers::GetAssetFullPath(fileName.c_str()) };
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputFile.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cout << "Model did not load!" << std::endl;
        return false;
    }

    // Vertices are stored in a vector as separate x, y, z floats, so vector size is divided by 3.
    meshVertices.resize(attrib.vertices.size() / 3);

    // Each shape can be thought of as a separate mesh in the file, so a sum of index
    // counts for all shapes is needed.
    size_t numOfIndices = 0;
    for (int shapeID = 0; shapeID < shapes.size(); shapeID++)
    {
        numOfIndices += shapes[shapeID].mesh.indices.size();
    }
    meshIndices.resize(numOfIndices);

    // Iterate over shapes in the file.
    size_t processedIndicesCount = 0;
    size_t countShapesToLoad = shapes.size();
    for (int shapeID = 0; shapeID < countShapesToLoad; shapeID++)
    {
        // Iterate over indices in the shape. They index into attrib.vertices as if
        // each xyz triple was one element (so max index will be attrib.vertices.size / 3)
        size_t shapeIndicesCount = shapes[shapeID].mesh.indices.size();
        for (int indexID = 0; indexID < shapeIndicesCount; indexID++) {
            meshIndices[processedIndicesCount + indexID] = static_cast<DWORD>(shapes[shapeID].mesh.indices[indexID].vertex_index);

            // Prepare vertex data to be put into our Vertex structure.
            float vertexX, vertexY, vertexZ, vertexU, vertexV, normalX, normalY, normalZ;

            // Three (x, y, z) attrib.verices entries per vertex.
            vertexX = attrib.vertices[3 * shapes[shapeID].mesh.indices[indexID].vertex_index];
            vertexY = attrib.vertices[3 * shapes[shapeID].mesh.indices[indexID].vertex_index + 1];
            vertexZ = attrib.vertices[3 * shapes[shapeID].mesh.indices[indexID].vertex_index + 2];

            // Two (u, v) attrib.texcoords entries per unwrapped vertex(!)
            if(attrib.texcoords.size() > 0) {
                vertexU = attrib.texcoords[2 * shapes[shapeID].mesh.indices[indexID].texcoord_index];
                vertexV = attrib.texcoords[2 * shapes[shapeID].mesh.indices[indexID].texcoord_index + 1];
            }
            else {
                vertexU = 0.f;
                vertexV = 0.f;
            }

            // Three (x, y, z) attrib.normals entries per face.
            if(attrib.normals.size() > 0) {
                normalX = attrib.normals[3 * shapes[shapeID].mesh.indices[indexID].normal_index];
                normalY = attrib.normals[3 * shapes[shapeID].mesh.indices[indexID].normal_index + 1];
                normalZ = attrib.normals[3 * shapes[shapeID].mesh.indices[indexID].normal_index + 2];
            }
            else {
                normalX = 0.f;
                normalY = 0.f;
                normalZ = 0.f;
            }

            Vertex vertex = {
                {vertexX, vertexY, vertexZ},
                {1.f, 1.f, 1.f, 1.f},
                {vertexU, vertexV},
                {normalX, normalY, normalZ}
            };
            meshVertices[meshIndices[processedIndicesCount + indexID]] = vertex;
        }
        processedIndicesCount += shapeIndicesCount;
    }

    return true;
}

void Mesh::CreateMeshVertexAndIndexBuffers(std::vector<Vertex> &meshVertices, std::vector<DWORD> &meshIndices)
{
    BufferMemoryManager buffMng;

    indexCount = static_cast<UINT>(meshIndices.size());

    UINT vertexBufferSize = static_cast<UINT>(meshVertices.size() * sizeof(Vertex));
    ComPtr<ID3D12Resource> vertexUploadBuffer;
    buffMng.AllocateBuffer(vertexUploadBuffer, vertexBufferSize, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
    buffMng.AllocateBuffer(vertexBuffer, vertexBufferSize, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);

    D3D12_SUBRESOURCE_DATA vertexData = {};
    vertexData.pData = reinterpret_cast<BYTE*>(meshVertices.data());
    vertexData.RowPitch = vertexBufferSize;
    vertexData.SlicePitch = vertexBufferSize;

    // We don't close the commandList and wait for it to finish as we have more commands to upload.
    buffMng.FillBuffer(vertexBuffer, vertexData, vertexUploadBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    // Initialize the vertex buffer view.
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

    UINT indexBufferSize = static_cast<UINT>(meshIndices.size() * sizeof(DWORD));
    ComPtr<ID3D12Resource> indexUploadBuffer;
    buffMng.AllocateBuffer(indexUploadBuffer, indexBufferSize, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD);
    buffMng.AllocateBuffer(indexBuffer, indexBufferSize, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_HEAP_TYPE_DEFAULT);

    D3D12_SUBRESOURCE_DATA indexData = {};
    indexData.pData = reinterpret_cast<BYTE*>(meshIndices.data());
    indexData.RowPitch = indexBufferSize;
    indexData.SlicePitch = indexBufferSize;

    buffMng.FillBuffer(indexBuffer, indexData, indexUploadBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    // Initialize the index buffer view.
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = indexBufferSize;
}
