using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Cors;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication1.Models;

namespace WebApplication1.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BoardDatumsController : ControllerBase
    {
        private readonly Comp3000TestContext _context;

        public BoardDatumsController(Comp3000TestContext context)
        {
            _context = context;
        }

        [EnableCors]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<BoardDatum>>> GetData()
        {


            var count = _context.BoardData.Count();
            var data = await _context.BoardData.Where(x => x.DataId > count - 20).ToListAsync();

            Console.WriteLine(count);

            return data;

        }

        // GET: api/BoardDatums/5
        [HttpGet("{id}")]
        public async Task<ActionResult<BoardDatum>> GetBoardDatum(int id)
        {
            var boardDatum = await _context.BoardData.FindAsync(id);

            if (boardDatum == null)
            {
                return NotFound();
            }

            return boardDatum;
        }

        // PUT: api/BoardDatums/5
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPut("{id}")]
        public async Task<IActionResult> PutBoardDatum(int id, BoardDatum boardDatum)
        {
            if (id != boardDatum.DataId)
            {
                return BadRequest();
            }

            _context.Entry(boardDatum).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!BoardDatumExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/BoardDatums
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<BoardDatum>> PostBoardDatum(BoardDatum boardDatum)
        {
            
            _context.BoardData.Add(boardDatum);
            await _context.SaveChangesAsync();

            return CreatedAtAction("GetBoardDatum", new { id = boardDatum.DataId }, boardDatum);
        }

        // DELETE: api/BoardDatums/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteBoardDatum(int id)
        {
            var boardDatum = await _context.BoardData.FindAsync(id);
            if (boardDatum == null)
            {
                return NotFound();
            }

            _context.BoardData.Remove(boardDatum);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool BoardDatumExists(int id)
        {
            return _context.BoardData.Any(e => e.DataId == id);
        }
    }
}
