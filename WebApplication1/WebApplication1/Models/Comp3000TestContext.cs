using System;
using System.Collections.Generic;
using Microsoft.EntityFrameworkCore;

namespace WebApplication1.Models;

public partial class Comp3000TestContext : DbContext
{
    public Comp3000TestContext()
    {
    }

    public Comp3000TestContext(DbContextOptions<Comp3000TestContext> options)
        : base(options)
    {
    }

    public virtual DbSet<BoardDatum> BoardData { get; set; }

    public virtual DbSet<Test> Tests { get; set; }

    protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
#warning To protect potentially sensitive information in your connection string, you should move it out of source code. You can avoid scaffolding the connection string by using the Name= syntax to read it from configuration - see https://go.microsoft.com/fwlink/?linkid=2131148. For more guidance on storing connection strings, see https://go.microsoft.com/fwlink/?LinkId=723263.
        => optionsBuilder.UseSqlServer("Data Source=localhost;Initial Catalog=comp3000_test;Persist Security Info=True;User ID=SA;Password=Comp3000;Trust Server Certificate=True");

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<BoardDatum>(entity =>
        {
            entity.HasKey(e => e.DataId).HasName("PK__Board_Da__C4E85284EBA8A815");

            entity.ToTable("Board_Data", "Comp3000");

            entity.Property(e => e.DataId).HasColumnName("Data_ID");
            entity.Property(e => e.Light).HasMaxLength(20);
            entity.Property(e => e.Pressure).HasMaxLength(20);
            entity.Property(e => e.Temperature).HasMaxLength(20);
        });

        modelBuilder.Entity<Test>(entity =>
        {
            entity.HasKey(e => e.DataId).HasName("PK__Test__C4E96E8CCA6FE6F3");

            entity.ToTable("Test", "Comp3000");

            entity.Property(e => e.DataId)
                .ValueGeneratedNever()
                .HasColumnName("Data_id");
            entity.Property(e => e.Temperature).HasMaxLength(50);
        });

        OnModelCreatingPartial(modelBuilder);
    }

    partial void OnModelCreatingPartial(ModelBuilder modelBuilder);
}
