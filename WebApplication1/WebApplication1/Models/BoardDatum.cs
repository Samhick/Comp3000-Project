using System;
using System.Collections.Generic;

namespace WebApplication1.Models;

public partial class BoardDatum
{
    public int DataId { get; set; }

    public string? Temperature { get; set; }

    public string? Pressure { get; set; }

    public string? Light { get; set; }
}
