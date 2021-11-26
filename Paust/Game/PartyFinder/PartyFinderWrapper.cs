using System.Collections.Generic;
using Paust.Game;
using Paust.Game.Data;

namespace Paust.PartyFinder
{
    internal class PartyFinderWrapper
    {
        public PartyFinderWrapper(PartyFinderPacketListing listing)
        {
            this.owner_name = SeString.ToString(listing.Name);
            this.description = SeString.ToString(listing.Description);

            this.remaining_seconds = listing.SecondsRemaining;

            this.content_id = (int)listing.Duty;
            this.is_eden =
                this.content_id == ContentIdConst.Instance.E9S ||
                this.content_id == ContentIdConst.Instance.E10S ||
                this.content_id == ContentIdConst.Instance.E11S ||
                this.content_id == ContentIdConst.Instance.E12S;

            this.is_roulette = listing.DutyType == 1;

            this.party_count = listing.NumParties;

            var is_24 = (listing.SearchArea & (1 << 2)) != 0;
            this.is_private  = (listing.SearchArea & (1 << 1)) != 0;
            this.is_24       = is_24;
            this.same_server = (listing.SearchArea & (1 << 3)) != 0;
            this.no_dup_job  = (listing.SearchArea & (1 << 5)) != 0;

            this.welcome_beginner = listing.BeginnersWelcome == 1;

            this.undersize_player = (listing.DutyFinderSettings & (1 << 0)) != 0;
            this.minimum_level = (listing.DutyFinderSettings & (1 << 1)) == 0 ? 0 : listing.MinimumItemLevel;
            this.silence_echo = (listing.DutyFinderSettings & (1 << 2)) != 0;

            this.item_rule = listing.LootRules;
            this.slot_count = listing.NumSlots;
            this.objective = listing.Objective;

            var lst = new List<SlotData>();
            if (!is_24)
            {
                for (int i = 0; i < listing.Slots.Length && i < listing.JobsPresent.Length; i++)
                {
                    lst.Add(new SlotData(listing, i));
                }
            }

            this.slot = lst.ToArray();
        }

        public string owner_name { get; }
        public string description { get; }

        public int remaining_seconds { get; }

        public int content_category { get; }

        public int content_id { get; }
        public bool is_eden { get; }

        public bool is_roulette { get; }

        public int party_count { get; }

        public bool is_private { get; }
        public bool is_24 { get; }
        public bool same_server { get; }
        public bool no_dup_job { get; }

        public bool welcome_beginner { get; }
        
        public bool undersize_player { get; }
        public int minimum_level { get; }
        public bool silence_echo { get; }

        public int item_rule { get; }

        public int slot_count { get; }

        public int objective { get; }

        public SlotData[] slot { get; }

        public class SlotData
        {
            public SlotData(PartyFinderPacketListing listing, int i)
            {
                var is_empty = listing.JobsPresent[i] == 0;

                var slot = listing.Slots[i];

                if (is_empty)
                {
                    this._tank = (slot & SlotConst.Instance._tank) != 0;

                    this._heal = (slot & SlotConst.Instance._heal) != 0;

                    this._deal = (slot & SlotConst.Instance._deal) != 0;
                    this._deal_meele = (slot & SlotConst.Instance._deal_meele) != 0;
                    this._deal_range = (slot & SlotConst.Instance._deal_range) != 0;
                    this._deal_caster = (slot & SlotConst.Instance._deal_caster) != 0;

                    this.gld = (slot & SlotConst.Instance.gld) != 0;
                    this.pgl = (slot & SlotConst.Instance.pgl) != 0;
                    this.mrd = (slot & SlotConst.Instance.mrd) != 0;
                    this.lnc = (slot & SlotConst.Instance.lnc) != 0;
                    this.arc = (slot & SlotConst.Instance.arc) != 0;
                    this.cnj = (slot & SlotConst.Instance.cnj) != 0;
                    this.thm = (slot & SlotConst.Instance.thm) != 0;
                    this.pld = (slot & SlotConst.Instance.pld) != 0;
                    this.mnk = (slot & SlotConst.Instance.mnk) != 0;
                    this.war = (slot & SlotConst.Instance.war) != 0;
                    this.drg = (slot & SlotConst.Instance.drg) != 0;
                    this.brd = (slot & SlotConst.Instance.brd) != 0;
                    this.whm = (slot & SlotConst.Instance.whm) != 0;
                    this.blm = (slot & SlotConst.Instance.blm) != 0;
                    this.acn = (slot & SlotConst.Instance.acn) != 0;
                    this.smn = (slot & SlotConst.Instance.smn) != 0;
                    this.sch = (slot & SlotConst.Instance.sch) != 0;
                    this.rog = (slot & SlotConst.Instance.rog) != 0;
                    this.nin = (slot & SlotConst.Instance.nin) != 0;
                    this.mch = (slot & SlotConst.Instance.mch) != 0;
                    this.drk = (slot & SlotConst.Instance.drk) != 0;
                    this.ast = (slot & SlotConst.Instance.ast) != 0;
                    this.sam = (slot & SlotConst.Instance.sam) != 0;
                    this.rdm = (slot & SlotConst.Instance.rdm) != 0;
                    this.blu = (slot & SlotConst.Instance.blu) != 0;
                    this.gnb = (slot & SlotConst.Instance.gnb) != 0;
                    this.dnc = (slot & SlotConst.Instance.dnc) != 0;
                }

                int available_jobs = 0;
                var s = listing.Slots[i];
                while (s != 0)
                {
                    available_jobs += (int)(s % 2);
                    s >>= 1;
                }

                this.in_slot = is_empty ? 0 : listing.JobsPresent[i];
                this._raw = is_empty ? 0 : (int)listing.Slots[i];
                this.available_jobs = is_empty ? available_jobs : 0;
            }


            public bool _tank { get; }

            public bool _heal { get; }

            public bool _deal { get; }
            public bool _deal_meele { get; }
            public bool _deal_range { get; }
            public bool _deal_caster { get; }

            public bool gld { get; }
            public bool pgl { get; }
            public bool mrd { get; }
            public bool lnc { get; }
            public bool arc { get; }
            public bool cnj { get; }
            public bool thm { get; }
            public bool pld { get; }
            public bool mnk { get; }
            public bool war { get; }
            public bool drg { get; }
            public bool brd { get; }
            public bool whm { get; }
            public bool blm { get; }
            public bool acn { get; }
            public bool smn { get; }
            public bool sch { get; }
            public bool rog { get; }
            public bool nin { get; }
            public bool mch { get; }
            public bool drk { get; }
            public bool ast { get; }
            public bool sam { get; }
            public bool rdm { get; }
            public bool blu { get; }
            public bool gnb { get; }
            public bool dnc { get; }

            public int in_slot { get; }
            public int _raw { get; }
            public int available_jobs { get; }
        }
    }
}
