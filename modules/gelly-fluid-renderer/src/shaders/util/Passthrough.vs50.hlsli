// When included, it stubs the main function which passes through to the pixel shader.

VS_OUTPUT main(VS_OUTPUT input)
{
    VS_OUTPUT output = input;
    return output;
}