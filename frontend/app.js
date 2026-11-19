/* ============================================================
   SHOWSPOT — Cinema & Regional Theatre Ticket Booking Platform
   BookMyShow-Inspired Interactive Frontend (Vanilla JS)
   Powered by C99 Backend Engine (Linked List, LIFO Stack, FIFO Queue)
   ============================================================ */

'use strict';

const API_BASE = (typeof window !== 'undefined' && window.location.origin && window.location.origin !== 'null')
  ? `${window.location.origin}/api`
  : 'http://localhost:8080/api';

const CAT_META = {
  1: { name: 'Commercial Movie', label: 'Movies', color: '#e11d48' },
  2: { name: 'Regional Theatre/Drama', label: 'Drama & Theatre', color: '#8b5cf6' },
  3: { name: 'Local Music Performance', label: 'Music & Concerts', color: '#0284c7' },
  4: { name: 'Folk & Traditional Dance', label: 'Dance', color: '#d97706' },
  5: { name: 'Cultural Event', label: 'Arts & Culture', color: '#059669' }
};

const APP_STATE = {
  events: [],
  bookings: [],
  overview: null,
  currentCity: 'Mumbai',
  currentPortal: 'buyer',
  currentView: 'home',
  selectedCategory: 'all',
  searchQuery: '',
  selectedEvent: null,
  targetSeatCount: 1,
  selectedSeats: [],
  recentTicket: null,
  heroSlideIndex: 0
};

async function api(path, method = 'GET', body = null) {
  const opts = {
    method,
    headers: { 'Content-Type': 'application/json' }
  };
  if (body !== null) {
    opts.body = JSON.stringify(body);
  }

  try {
    const res = await fetch(`${API_BASE}${path}`, opts);
    const text = await res.text();
    let data;
    try {
      data = JSON.parse(text);
    } catch {
      data = { message: text };
    }

    if (!res.ok) {
      const msg = (data && data.message) ? data.message : `Error ${res.status}`;
      showToast(msg, 'error');
      return null;
    }
    return data;
  } catch (err) {
    showToast(`Network error: ${err.message}. Ensure C server is running on port 8080.`, 'error');
    return null;
  }
}

function showToast(message, type = 'info') {
  const container = document.getElementById('toast-container');
  if (!container) return;

  const toast = document.createElement('div');
  toast.className = `bms-toast ${type}`;
  toast.innerHTML = `<span class="toast-dot ${type}"></span><span>${esc(message)}</span>`;
  container.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(40px)';
    toast.style.transition = 'all 0.25s ease';
    setTimeout(() => toast.remove(), 260);
  }, 4000);
}

function openModal({ title, bodyHTML, actions = [] }) {
  closeModal();
  const container = document.getElementById('modal-container');
  if (!container) return;

  const overlay = document.createElement('div');
  overlay.className = 'modal-overlay';
  overlay.id = 'active-modal';

  const actionsHTML = actions.map(a =>
    `<button class="${a.cls || 'btn-ghost'}" data-action="${a.id}">${esc(a.label)}</button>`
  ).join('');

  overlay.innerHTML = `
    <div class="modal-box">
      <div class="modal-header">
        <div class="modal-title">${esc(title)}</div>
        <button class="modal-close-btn" id="modal-close-x">✕</button>
      </div>
      <div class="modal-body">${bodyHTML}</div>
      ${actions.length ? `<div class="modal-actions">${actionsHTML}</div>` : ''}
    </div>`;

  container.appendChild(overlay);

  overlay.addEventListener('click', e => {
    if (e.target === overlay) closeModal();
  });
  document.getElementById('modal-close-x').addEventListener('click', closeModal);

  overlay.querySelectorAll('[data-action]').forEach(btn => {
    const act = actions.find(a => a.id === btn.dataset.action);
    if (act && act.onClick) btn.addEventListener('click', act.onClick);
  });
}

function closeModal() {
  const m = document.getElementById('active-modal');
  if (m) m.remove();
}

function esc(str) {
  if (str === null || str === undefined) return '';
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

function formatPrice(p) {
  if (p === undefined || p === null || isNaN(p)) return '₹0.00';
  return `₹${Number(p).toFixed(2)}`;
}

function getEventPosterArt(ev) {
  const title = ev.title || '';
  const cat = ev.category || 1;

  if (title.includes('Inception')) {
    return {
      posterUrl: 'https://image.tmdb.org/t/p/w500/oYuLEt3zVCKq57qu2F8dT7NIa6f.jpg',
      bannerUrl: 'https://image.tmdb.org/t/p/original/8ZTVqvKDQ8emSGUEMjsS4yHAwrp.jpg',
      genre: 'Sci-Fi / Action / Thriller',
      language: 'English, Hindi',
      rating: '8.8',
      votes: '2.4M',
      badge: 'BLOCKBUSTER',
      bg: '#0a0e17'
    };
  }
  if (title.includes('Kantara')) {
    return {
      posterUrl: 'https://image.tmdb.org/t/p/w500/jIsKmkxMzdCZ0Ux1GVSnu8m6Na6.jpg',
      bannerUrl: 'https://m.media-amazon.com/images/S/pv-target-images/257bacd00bf052b2d4ae1e1259980d4af71d804a5d969c0d7a87436cf9c4da92._SX1080_FMjpg_.jpg',
      genre: 'Action / Mythological Drama',
      language: 'Kannada, Hindi',
      rating: '9.1',
      votes: '480K',
      badge: 'TRENDING',
      bg: '#1c1917'
    };
  }
  if (title.includes('Natsamrat')) {
    return {
      posterUrl: 'https://m.media-amazon.com/images/S/pv-target-images/3c03f1dd671bfbfcf71e85ebd693e0b1cce670353bbd871d0b026ca30e39e27a._SX1080_FMjpg_.jpg',
      bannerUrl: 'https://m.media-amazon.com/images/S/pv-target-images/3c03f1dd671bfbfcf71e85ebd693e0b1cce670353bbd871d0b026ca30e39e27a._SX1080_FMjpg_.jpg',
      genre: 'Classic Theatre / Marathi Drama',
      language: 'Marathi',
      rating: '9.4',
      votes: '120K',
      badge: 'CRITICALLY ACCLAIMED',
      bg: '#1c1512'
    };
  }
  if (title.includes('Baul') || title.includes('Sufi')) {
    return {
      posterUrl: 'https://images.unsplash.com/photo-1543783207-ec64e4d95325?w=600&auto=format&fit=crop&q=80',
      bannerUrl: 'https://images.unsplash.com/photo-1511671782779-c97d3d27a1d4?w=1400&auto=format&fit=crop&q=80',
      genre: 'Spiritual Sufi & Baul Folk Music',
      language: 'Bengali, Hindi',
      rating: '9.0',
      votes: '45K',
      badge: 'LIVE CONCERT',
      bg: '#15152a'
    };
  }
  if (title.includes('Ghoomar') || title.includes('Dance')) {
    return {
      posterUrl: 'https://images.unsplash.com/photo-1609137144813-7d9921338f24?w=600&auto=format&fit=crop&q=80',
      bannerUrl: 'https://images.unsplash.com/photo-1516450360452-9312f5e86fc7?w=1400&auto=format&fit=crop&q=80',
      genre: 'Traditional Rajasthani Folk Dance',
      language: 'Rajasthani',
      rating: '8.9',
      votes: '32K',
      badge: 'FOLK HERITAGE',
      bg: '#251616'
    };
  }
  if (title.includes('Sold-Out') || title.includes('Cultural Gala')) {
    return {
      posterUrl: 'https://images.unsplash.com/photo-1469488865564-c2de10f69f96?w=600&auto=format&fit=crop&q=80',
      bannerUrl: 'https://images.unsplash.com/photo-1514525253161-7a46d19cd819?w=1400&auto=format&fit=crop&q=80',
      genre: 'Grand Cultural Auditorium Gala',
      language: 'Multi-lingual',
      rating: '9.6',
      votes: '89K',
      badge: 'HOUSEFULL',
      bg: '#111827'
    };
  }
  if (title.includes('Shakuntala')) {
    return {
      posterUrl: 'https://m.media-amazon.com/images/S/pv-target-images/c2ef1afae48784d71caba8b5cc72d05301616de16b8b3d52207890b13ee4da31._SX1080_FMjpg_.jpg',
      bannerUrl: 'https://m.media-amazon.com/images/S/pv-target-images/c2ef1afae48784d71caba8b5cc72d05301616de16b8b3d52207890b13ee4da31._SX1080_FMjpg_.jpg',
      genre: 'Kalidasa Sanskrit Musical Drama',
      language: 'Sanskrit, Hindi',
      rating: '9.2',
      votes: '64K',
      badge: 'HERITAGE CLASSIC',
      bg: '#18121a'
    };
  }

  const meta = CAT_META[cat] || CAT_META[1];
  return {
    posterUrl: '',
    bannerUrl: '',
    genre: meta.name,
    language: 'Hindi, English',
    rating: '8.5',
    votes: '10K',
    badge: 'FEATURED',
    bg: '#18181b'
  };
}

async function loadAllData() {
  const events = await api('/events');
  if (Array.isArray(events)) {
    APP_STATE.events = events;
  } else if (events && Array.isArray(events.events)) {
    APP_STATE.events = events.events;
  } else if (events && Array.isArray(events.value)) {
    APP_STATE.events = events.value;
  }

  const bookings = await api('/bookings');
  if (Array.isArray(bookings)) {
    APP_STATE.bookings = bookings;
    updateBookingsCountBadge(bookings.length);
  } else if (bookings && Array.isArray(bookings.bookings)) {
    APP_STATE.bookings = bookings.bookings;
    updateBookingsCountBadge(bookings.bookings.length);
  } else if (bookings && Array.isArray(bookings.value)) {
    APP_STATE.bookings = bookings.value;
    updateBookingsCountBadge(bookings.value.length);
  }

  const overview = await api('/overview');
  if (overview) {
    APP_STATE.overview = overview;
  }
}

function updateBookingsCountBadge(count) {
  const badge = document.getElementById('bookings-count-badge');
  if (!badge) return;
  if (count > 0) {
    badge.textContent = count;
    badge.style.display = 'inline-block';
  } else {
    badge.style.display = 'none';
  }
}

function renderApp() {
  const root = document.getElementById('app-root');
  if (!root) return;
  window.scrollTo({ top: 0, behavior: 'smooth' });

  const catSubnav = document.getElementById('category-subnav');
  if (catSubnav) {
    catSubnav.style.display = (APP_STATE.currentPortal === 'buyer') ? 'block' : 'none';
  }

  document.querySelectorAll('.portal-pill').forEach(pill => {
    pill.classList.toggle('active', pill.dataset.portal === APP_STATE.currentPortal);
  });

  if (APP_STATE.currentPortal === 'artist') {
    renderArtistView(root);
  } else if (APP_STATE.currentPortal === 'officer') {
    renderOfficerView(root);
  } else {
    if (APP_STATE.currentView === 'seat-view') {
      renderSeatMatrixView(root, APP_STATE.selectedEvent);
    } else if (APP_STATE.currentView === 'ticket-pass') {
      renderTicketPassView(root, APP_STATE.recentTicket);
    } else if (APP_STATE.currentView === 'bookings') {
      renderMyBookingsView(root);
    } else {
      renderHomeDiscoveryView(root);
    }
  }
}

function renderHomeDiscoveryView(container) {
  const featured = APP_STATE.events.slice(0, 3);
  const slide = featured[APP_STATE.heroSlideIndex % (featured.length || 1)] || featured[0];

  let heroHTML = '';
  if (slide) {
    const art = getEventPosterArt(slide);
    const available = slide.available_seats !== undefined ? slide.available_seats : slide.availableSeats;
    const bgStyle = art.bannerUrl ? `background-image: url('${art.bannerUrl}');` : `background: ${art.bg || '#111827'};`;
    heroHTML = `
      <div class="hero-carousel-container">
        <div class="hero-banner-card" style="${bgStyle}">
          <div class="hero-banner-overlay"></div>
          <div class="hero-banner-content">
            <div class="hero-badge-group">
              <span class="hero-pill ${slide.category === 1 ? 'commercial' : 'regional'}">
                ${esc(art.badge)}
              </span>
              <span class="hero-pill format">${esc(art.genre)}</span>
              <span class="hero-pill rating"><span class="rating-star">★</span> ${art.rating} (${art.votes})</span>
            </div>
            <h1 class="hero-title">${esc(slide.title)}</h1>
            <div class="hero-meta">
              <span>${esc(slide.artist)}</span>
              <span>&bull;</span>
              <span>${esc(slide.venue)}</span>
              <span>&bull;</span>
              <span>${esc(slide.time)}</span>
            </div>
            <p class="hero-desc">
              Experience live performing arts backed by 3rd semester C99 Data Structures: dynamic linked lists, 10×12 seat matrices, LIFO cancellation undo stack and FIFO waiting-list queues.
            </p>
            <div class="hero-actions">
              ${available > 0
                ? `<button class="btn-book-hero" data-event-id="${slide.id}">Book Tickets</button>`
                : `<button class="btn-book-hero waitlist" data-waitlist-id="${slide.id}">Join Waiting List</button>`}
              <div class="hero-pricing-tag">
                Tickets from <strong>${formatPrice(slide.regular_price || slide.regularPrice)}</strong>
              </div>
            </div>
          </div>
          <button class="hero-nav-arrow prev" id="hero-prev-btn" aria-label="Previous">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="15 18 9 12 15 6"></polyline></svg>
          </button>
          <button class="hero-nav-arrow next" id="hero-next-btn" aria-label="Next">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><polyline points="9 18 15 12 9 6"></polyline></svg>
          </button>
          <div class="hero-dots">
            ${featured.map((_, i) => `<button class="hero-dot ${i === (APP_STATE.heroSlideIndex % featured.length) ? 'active' : ''}" data-slide="${i}"></button>`).join('')}
          </div>
        </div>
      </div>`;
  }

  let filterBannerHTML = '';
  let filteredEvents = APP_STATE.events;

  if (APP_STATE.searchQuery.trim()) {
    const q = APP_STATE.searchQuery.toLowerCase().trim();
    filteredEvents = filteredEvents.filter(ev =>
      (ev.title && ev.title.toLowerCase().includes(q)) ||
      (ev.artist && ev.artist.toLowerCase().includes(q)) ||
      (ev.venue && ev.venue.toLowerCase().includes(q))
    );
    filterBannerHTML = `
      <div style="background:#ffffff;border-radius:10px;padding:16px 20px;margin-top:20px;display:flex;align-items:center;justify-content:space-between;border:1px solid #e5e7eb;box-shadow:var(--bms-shadow-sm);">
        <div>
          <span style="font-size:14px;color:var(--bms-text-muted);">Search results for:</span>
          <strong style="font-size:16px;color:var(--bms-text-main);margin-left:6px;">"${esc(APP_STATE.searchQuery)}"</strong>
          <span style="font-size:12px;background:#f3f4f6;padding:2px 8px;border-radius:12px;margin-left:8px;font-weight:600;">${filteredEvents.length} found</span>
        </div>
        <button class="btn-ghost" id="clear-search-filter" style="padding:6px 12px;font-size:12px;">Clear Search</button>
      </div>`;
  } else if (APP_STATE.selectedCategory !== 'all') {
    const catNum = Number(APP_STATE.selectedCategory);
    if (!isNaN(catNum) && catNum >= 1 && catNum <= 5) {
      filteredEvents = filteredEvents.filter(ev => ev.category === catNum);
      const meta = CAT_META[catNum];
      filterBannerHTML = `
        <div style="background:#ffffff;border-radius:8px;padding:16px 20px;margin-top:20px;display:flex;align-items:center;justify-content:space-between;border:1px solid #e5e7eb;box-shadow:var(--bms-shadow-sm);">
          <div>
            <h2 style="font-size:18px;font-weight:700;color:var(--bms-text-main);">${esc(meta.name)}</h2>
            <p style="font-size:12px;color:var(--bms-text-muted);margin-top:2px;">${filteredEvents.length} active shows available in ${esc(APP_STATE.currentCity)}</p>
          </div>
          <button class="btn-ghost" id="clear-cat-filter" style="padding:6px 14px;font-size:12px;">View All Shows</button>
        </div>`;
    } else {
      const unsupportedNames = {
        sports: 'Sports & Games',
        comedy: 'Stand-up Comedy',
        workshops: 'Creative Workshops & Masterclasses'
      };
      const catTitle = unsupportedNames[APP_STATE.selectedCategory] || 'Category';
      container.innerHTML = `
        ${heroHTML}
        <div style="background:#ffffff;border-radius:10px;padding:48px 24px;margin-top:30px;text-align:center;box-shadow:var(--bms-shadow-sm);border:1px solid var(--bms-border);">
          <div style="width:48px;height:48px;border-radius:50%;background:#f3f4f6;display:flex;align-items:center;justify-content:center;margin:0 auto 16px;color:var(--bms-text-muted);">
            <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="8" x2="12" y2="12"></line><line x1="12" y1="16" x2="12.01" y2="16"></line></svg>
          </div>
          <h2 style="font-size:20px;font-weight:700;color:var(--bms-text-main);margin-bottom:8px;">No Live ${esc(catTitle)} Available</h2>
          <p style="font-size:13.5px;color:var(--bms-text-muted);max-width:540px;margin:0 auto 20px;line-height:1.5;">
            The C99 backend currently manages Categories 1 through 5 (Commercial Movies, Regional Theatre, Music, Folk Dance, and Cultural Extravaganzas). No upcoming events are scheduled under <strong>${esc(catTitle)}</strong> for ${esc(APP_STATE.currentCity)} at this moment.
          </p>
          <button class="btn-primary" id="btn-browse-all-back">Explore Available Movies &amp; Theatre</button>
        </div>`;

      wireHeroEvents(container);
      const btnBack = container.querySelector('#btn-browse-all-back');
      if (btnBack) {
        btnBack.addEventListener('click', () => {
          APP_STATE.selectedCategory = 'all';
          updateCategoryTabs();
          renderApp();
        });
      }
      return;
    }
  }

  const recEvents = filteredEvents.slice(0, 4);
  const movieEvents = filteredEvents.filter(ev => ev.category === 1);
  const theatreEvents = filteredEvents.filter(ev => ev.category === 2);
  const musicDanceEvents = filteredEvents.filter(ev => ev.category === 3 || ev.category === 4 || ev.category === 5);
  const soldOutEvents = filteredEvents.filter(ev => {
    const av = ev.available_seats !== undefined ? ev.available_seats : ev.availableSeats;
    return av === 0;
  });

  let sectionsHTML = '';

  if (APP_STATE.searchQuery.trim() || APP_STATE.selectedCategory !== 'all') {
    sectionsHTML = `
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Shows Matching Your Filter</h2>
            <span class="section-subtitle">Real-time availability from C backend memory</span>
          </div>
        </div>
        ${renderCardGridHTML(filteredEvents)}
      </section>`;
  } else {
    sectionsHTML = `
      <!-- Recommended for You -->
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Recommended for You</h2>
            <span class="section-subtitle">Curated top entertainment in ${esc(APP_STATE.currentCity)}</span>
          </div>
          <a class="section-see-all" data-see-cat="all">See All ›</a>
        </div>
        ${renderCardGridHTML(recEvents)}
      </section>

      <!-- Popular Movies -->
      ${movieEvents.length ? `
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Popular Movies</h2>
            <span class="section-subtitle">Commercial box office releases</span>
          </div>
          <a class="section-see-all" data-see-cat="1">See All (${movieEvents.length}) ›</a>
        </div>
        ${renderCardGridHTML(movieEvents)}
      </section>` : ''}

      <!-- Regional Theatre & Drama -->
      ${theatreEvents.length ? `
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Regional Theatre &amp; Drama</h2>
            <span class="section-subtitle">Empowering local regional stage artists &amp; classical plays</span>
          </div>
          <a class="section-see-all" data-see-cat="2">See All (${theatreEvents.length}) ›</a>
        </div>
        ${renderCardGridHTML(theatreEvents)}
      </section>` : ''}

      <!-- Live Music & Folk Arts -->
      ${musicDanceEvents.length ? `
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Live Music &amp; Folk Arts</h2>
            <span class="section-subtitle">Sufi, Baul, Ghoomar, Kalbeliya &amp; indigenous troupe performances</span>
          </div>
          <a class="section-see-all" data-see-cat="3">See All (${musicDanceEvents.length}) ›</a>
        </div>
        ${renderCardGridHTML(musicDanceEvents)}
      </section>` : ''}

      <!-- Trending & Housefull (FIFO Queue) -->
      ${soldOutEvents.length ? `
      <section class="showcase-section">
        <div class="section-header">
          <div class="section-title-wrap">
            <h2 class="section-title">Housefull Shows &bull; Waiting List Active</h2>
            <span class="section-subtitle">Sold-out shows protected by FIFO Queue data structures</span>
          </div>
        </div>
        ${renderCardGridHTML(soldOutEvents)}
      </section>` : ''}`;
  }

  container.innerHTML = `
    ${heroHTML}
    ${filterBannerHTML}
    ${sectionsHTML}
  `;

  wireHeroEvents(container);
  wireCardButtons(container);

  const clearSearch = container.querySelector('#clear-search-filter');
  if (clearSearch) {
    clearSearch.addEventListener('click', () => {
      APP_STATE.searchQuery = '';
      const inp = document.getElementById('global-search-input');
      if (inp) inp.value = '';
      renderApp();
    });
  }

  const clearCat = container.querySelector('#clear-cat-filter');
  if (clearCat) {
    clearCat.addEventListener('click', () => {
      APP_STATE.selectedCategory = 'all';
      updateCategoryTabs();
      renderApp();
    });
  }

  container.querySelectorAll('[data-see-cat]').forEach(link => {
    link.addEventListener('click', () => {
      APP_STATE.selectedCategory = link.dataset.seeCat;
      updateCategoryTabs();
      renderApp();
    });
  });
}

function renderCardGridHTML(eventsList) {
  if (!eventsList || !eventsList.length) {
    return `
      <div style="background:#ffffff;border-radius:8px;padding:36px;text-align:center;border:1px solid #e5e7eb;">
        <p style="font-size:14px;color:var(--bms-text-muted);">No events found matching this selection.</p>
      </div>`;
  }

  return `
    <div class="events-grid">
      ${eventsList.map(ev => renderEventCardHTML(ev)).join('')}
    </div>`;
}

function renderEventCardHTML(ev) {
  const art = getEventPosterArt(ev);
  const available = ev.available_seats !== undefined ? ev.available_seats : ev.availableSeats;
  const isHousefull = available === 0;
  const cat = CAT_META[ev.category] || { label: 'Entertainment' };
  const hasPoster = Boolean(art.posterUrl && art.posterUrl.trim());

  return `
    <div class="event-card" data-event-id="${ev.id}">
      <div class="card-poster">
        ${hasPoster ? `
          <img src="${art.posterUrl}" alt="${esc(ev.title)}" class="poster-img" loading="lazy" onerror="this.style.display='none'; this.nextElementSibling.style.display='flex';" />
        ` : ''}
        <div class="poster-fallback" style="${hasPoster ? 'display:none;' : 'display:flex;'} background:${art.bg};">
          <div class="fallback-tag">${esc(art.badge)}</div>
          <div class="fallback-title">${esc(ev.title)}</div>
          <div class="fallback-genre">${esc(art.genre)}</div>
        </div>
        <div class="poster-badge-top">
          <span class="badge-tag cat-${ev.category || 1}">${esc(cat.label)}</span>
        </div>
        <div class="poster-rating-bar">
          <span class="rating-val"><span class="rating-star">★</span> ${art.rating}/10</span>
          <span class="rating-votes">${art.votes}</span>
        </div>
      </div>
      <div class="card-body">
        <h3 class="card-title" title="${esc(ev.title)}">${esc(ev.title)}</h3>
        <div class="card-genre">${esc(art.genre)}</div>
        <div class="card-venue-time">
          <span>${esc(ev.venue || 'Main Stage')}</span>
          <span>&bull;</span>
          <span>${esc(ev.time || 'Evening')}</span>
        </div>
        <div class="card-status-row">
          <div class="card-price">From <strong>${formatPrice(ev.regular_price || ev.regularPrice)}</strong></div>
          <div class="seat-status-pill ${isHousefull ? 'full' : 'available'}">
            <span class="status-dot"></span>
            ${isHousefull ? 'Housefull' : `${available} seats left`}
          </div>
        </div>
        <div class="card-footer">
          ${isHousefull
            ? `<button class="btn-waitlist-card" data-action-waitlist="${ev.id}">Join Waiting List</button>`
            : `<button class="btn-book-card" data-action-book="${ev.id}">Book Tickets</button>`}
        </div>
      </div>
    </div>`;
}

function wireHeroEvents(container) {
  const prev = container.querySelector('#hero-prev-btn');
  const next = container.querySelector('#hero-next-btn');

  if (prev) {
    prev.addEventListener('click', () => {
      APP_STATE.heroSlideIndex = (APP_STATE.heroSlideIndex - 1 + 3) % 3;
      renderApp();
    });
  }
  if (next) {
    next.addEventListener('click', () => {
      APP_STATE.heroSlideIndex = (APP_STATE.heroSlideIndex + 1) % 3;
      renderApp();
    });
  }

  container.querySelectorAll('.hero-dot').forEach(dot => {
    dot.addEventListener('click', () => {
      APP_STATE.heroSlideIndex = Number(dot.dataset.slide);
      renderApp();
    });
  });

  const heroBook = container.querySelector('.btn-book-hero');
  if (heroBook) {
    heroBook.addEventListener('click', () => {
      if (heroBook.dataset.eventId) {
        startBookingFlow(Number(heroBook.dataset.eventId));
      } else if (heroBook.dataset.waitlistId) {
        openWaitlistModal(Number(heroBook.dataset.waitlistId));
      }
    });
  }
}

function wireCardButtons(container) {
  container.querySelectorAll('[data-action-book]').forEach(btn => {
    btn.addEventListener('click', e => {
      e.stopPropagation();
      startBookingFlow(Number(btn.dataset.actionBook));
    });
  });

  container.querySelectorAll('[data-action-waitlist]').forEach(btn => {
    btn.addEventListener('click', e => {
      e.stopPropagation();
      openWaitlistModal(Number(btn.dataset.actionWaitlist));
    });
  });

  container.querySelectorAll('.event-card').forEach(card => {
    card.addEventListener('click', () => {
      const eid = Number(card.dataset.eventId);
      startBookingFlow(eid);
    });
  });
}

async function startBookingFlow(eventId) {
  const ev = APP_STATE.events.find(e => e.id === eventId);
  if (!ev) {
    showToast('Event not found.', 'error');
    return;
  }

  const available = ev.available_seats !== undefined ? ev.available_seats : ev.availableSeats;
  if (available === 0) {
    openWaitlistModal(eventId);
    return;
  }

  promptSeatQuantity(ev);
}

function promptSeatQuantity(eventObj) {
  const available = eventObj.available_seats !== undefined ? eventObj.available_seats : eventObj.availableSeats;
  const maxSelectable = Math.min(available, 10);
  let currentQty = Math.min(2, maxSelectable);
  if (currentQty < 1) currentQty = 1;

  const quantities = [1, 2, 3, 4, 5, 6, 8, 10].filter(q => q <= maxSelectable);
  if (!quantities.includes(currentQty)) quantities.unshift(currentQty);
  quantities.sort((a, b) => a - b);

  function getModalBodyHTML(qty) {
    return `
      <div class="qty-modal-content">
        <h3 class="qty-prompt-title">How many seats would you like to book?</h3>
        <p class="qty-prompt-desc">${esc(eventObj.title)} &bull; ${esc(eventObj.venue)} (${available} seats available)</p>
        
        <div class="qty-pills-grid" id="qty-pills-container">
          ${quantities.map(q => `
            <button class="qty-pill-btn ${q === qty ? 'active' : ''}" data-qty="${q}">${q}</button>
          `).join('')}
        </div>

        <div class="qty-stepper-wrap">
          <button class="qty-step-btn" id="btn-qty-minus" ${qty <= 1 ? 'disabled' : ''}>−</button>
          <span class="qty-step-val" id="qty-stepper-display">${qty}</span>
          <button class="qty-step-btn" id="btn-qty-plus" ${qty >= maxSelectable ? 'disabled' : ''}>+</button>
        </div>
      </div>
    `;
  }

  openModal({
    title: 'Select Number of Seats',
    bodyHTML: getModalBodyHTML(currentQty),
    actions: [
      { id: 'cancel', label: 'Cancel', cls: 'btn-ghost', onClick: closeModal },
      {
        id: 'proceed',
        label: `Select ${currentQty} Seat${currentQty > 1 ? 's' : ''} →`,
        cls: 'btn-primary',
        onClick: () => {
          closeModal();
          APP_STATE.selectedEvent = eventObj;
          APP_STATE.targetSeatCount = currentQty;
          APP_STATE.selectedSeats = [];
          APP_STATE.currentView = 'seat-view';
          renderApp();
        }
      }
    ]
  });

  function updateQtyUI(newQty) {
    currentQty = Math.max(1, Math.min(newQty, maxSelectable));
    const container = document.getElementById('qty-pills-container');
    if (container) {
      container.querySelectorAll('.qty-pill-btn').forEach(btn => {
        btn.classList.toggle('active', Number(btn.dataset.qty) === currentQty);
      });
    }
    const display = document.getElementById('qty-stepper-display');
    if (display) display.textContent = currentQty;
    const btnMinus = document.getElementById('btn-qty-minus');
    if (btnMinus) btnMinus.disabled = (currentQty <= 1);
    const btnPlus = document.getElementById('btn-qty-plus');
    if (btnPlus) btnPlus.disabled = (currentQty >= maxSelectable);

    const proceedBtn = document.querySelector('[data-action="proceed"]');
    if (proceedBtn) {
      proceedBtn.textContent = `Select ${currentQty} Seat${currentQty > 1 ? 's' : ''} →`;
    }
  }

  const modalEl = document.getElementById('active-modal');
  if (modalEl) {
    modalEl.addEventListener('click', e => {
      const pill = e.target.closest('.qty-pill-btn');
      if (pill) {
        updateQtyUI(Number(pill.dataset.qty));
      }
      if (e.target.id === 'btn-qty-minus') {
        updateQtyUI(currentQty - 1);
      }
      if (e.target.id === 'btn-qty-plus') {
        updateQtyUI(currentQty + 1);
      }
    });
  }
}

async function renderSeatMatrixView(container, eventObj) {
  if (!eventObj) {
    APP_STATE.currentView = 'home';
    renderApp();
    return;
  }

  container.innerHTML = `
    <div style="padding:40px;text-align:center;">
      <div class="loading-spinner" style="width:36px;height:36px;border:3px solid #e5e7eb;border-top-color:#e8192c;border-radius:50%;animation:spin 0.75s linear infinite;margin:0 auto 16px;"></div>
      <p style="color:var(--bms-text-muted);">Loading 10×12 auditorium seat matrix from C backend…</p>
    </div>`;

  const seatData = await api(`/seats?eventId=${eventObj.id}`);
  if (!seatData || !seatData.seats) {
    showToast('Could not load seat matrix for this event.', 'error');
    APP_STATE.currentView = 'home';
    renderApp();
    return;
  }

  const targetCount = APP_STATE.targetSeatCount || 1;
  const seats = seatData.seats;
  const vipRows = ['A', 'B'];
  const regRows = ['C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'];

  const rowsVipHTML = vipRows.map(rChar => renderSingleRowHTML(rChar, seats, 'VIP')).join('');
  const rowsRegHTML = regRows.map(rChar => renderSingleRowHTML(rChar, seats, 'Regular')).join('');

  const vipPrice = eventObj.vip_price || eventObj.vipPrice;
  const regPrice = eventObj.regular_price || eventObj.regularPrice;

  container.innerHTML = `
    <div class="audi-screen-view">
      <div class="audi-header">
        <div class="audi-event-info">
          <button class="btn-back-audi" id="btn-back-home-audi">← Back to Shows</button>
          <div class="audi-title-wrap">
            <h2>${esc(eventObj.title)}</h2>
            <div class="audi-meta-text">
              <span>${esc(eventObj.artist)}</span>
              <span>&bull;</span>
              <span>${esc(eventObj.venue)}</span>
              <span>&bull;</span>
              <span>${esc(eventObj.time)}</span>
            </div>
          </div>
        </div>
        <div class="audi-pricing-badges">
          <div class="pricing-badge vip">
            <span>RECLINER</span>
            <strong>${formatPrice(vipPrice)}</strong>
          </div>
          <div class="pricing-badge reg">
            <span>EXECUTIVE</span>
            <strong>${formatPrice(regPrice)}</strong>
          </div>
        </div>
      </div>

      <div class="audi-selection-status-bar">
        <div class="audi-status-left">
          <span class="status-badge-count" id="status-badge-count">Step 2: Select ${targetCount} Seat${targetCount > 1 ? 's' : ''}</span>
          <span class="status-text-live" id="selection-status-live">0 of ${targetCount} selected (${targetCount} remaining)</span>
        </div>
        <button class="status-btn-change-qty" id="btn-change-qty-seatview">Change Quantity (${targetCount})</button>
      </div>

      <div class="audi-screen-bar">
        <div class="screen-curved-arc"></div>
        <div class="screen-label">ALL EYES THIS WAY • SCREEN 1</div>
      </div>

      <div class="seat-matrix-wrapper">
        <div class="seat-tier-section">
          <div class="tier-header">
            <span class="tier-name">VIP RECLINER (ROWS A-B)</span>
            <span class="tier-price">${formatPrice(vipPrice)}</span>
          </div>
          ${rowsVipHTML}
        </div>

        <div class="seat-tier-section">
          <div class="tier-header">
            <span class="tier-name">EXECUTIVE (ROWS C-J)</span>
            <span class="tier-price">${formatPrice(regPrice)}</span>
          </div>
          ${rowsRegHTML}
        </div>

        <div class="seat-legend-bar">
          <div class="legend-item"><div class="legend-swatch reg"></div> Available (${formatPrice(regPrice)})</div>
          <div class="legend-item"><div class="legend-swatch vip"></div> Recliner (${formatPrice(vipPrice)})</div>
          <div class="legend-item"><div class="legend-swatch selected"></div> Selected</div>
          <div class="legend-item"><div class="legend-swatch booked"></div> Sold Out</div>
        </div>
      </div>
    </div>

    <!-- Floating Bottom Bar -->
    <div class="bottom-checkout-bar" id="bottom-checkout-bar" style="display:flex;">
      <div class="checkout-summary">
        <div class="checkout-seat-info">
          <span class="checkout-seat-label">Selected Seats</span>
          <span class="checkout-seat-val" id="checkout-seat-val">No seats selected</span>
        </div>
        <div class="checkout-price-info">
          <span class="checkout-price-label">Total Amount</span>
          <span class="checkout-price-val" id="checkout-price-val">${formatPrice(0)}</span>
        </div>
      </div>
      <button class="btn-proceed-book" id="btn-proceed-book" disabled style="opacity:0.6;cursor:not-allowed;">
        Select ${targetCount} Seat${targetCount > 1 ? 's' : ''} to Proceed
      </button>
    </div>`;

  const btnBack = container.querySelector('#btn-back-home-audi');
  if (btnBack) {
    btnBack.addEventListener('click', () => {
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  const btnChangeQty = container.querySelector('#btn-change-qty-seatview');
  if (btnChangeQty) {
    btnChangeQty.addEventListener('click', () => {
      promptSeatQuantity(eventObj);
    });
  }

  function updateCheckoutUI() {
    const selected = APP_STATE.selectedSeats;
    const count = selected.length;
    const remaining = targetCount - count;

    const statusBadge = document.getElementById('status-badge-count');
    const statusLive = document.getElementById('selection-status-live');
    const seatVal = document.getElementById('checkout-seat-val');
    const priceVal = document.getElementById('checkout-price-val');
    const proceedBtn = document.getElementById('btn-proceed-book');

    const vipCount = selected.filter(s => s.isVip).length;
    const regCount = selected.filter(s => !s.isVip).length;
    const vipTotal = vipCount * vipPrice;
    const regTotal = regCount * regPrice;
    const grandTotal = vipTotal + regTotal;

    if (statusBadge && statusLive) {
      if (remaining === 0) {
        statusBadge.className = 'status-badge-count ready';
        statusBadge.textContent = 'All Seats Selected';
        statusLive.textContent = `${count} of ${targetCount} selected — Ready for Checkout!`;
      } else {
        statusBadge.className = 'status-badge-count';
        statusBadge.textContent = `Step 2: Select ${targetCount} Seat${targetCount > 1 ? 's' : ''}`;
        statusLive.textContent = `${count} of ${targetCount} selected (${remaining} remaining)`;
      }
    }

    if (seatVal && priceVal && proceedBtn) {
      if (count === 0) {
        seatVal.textContent = 'No seats selected';
        priceVal.textContent = formatPrice(0);
        proceedBtn.disabled = true;
        proceedBtn.textContent = `Select ${targetCount} Seat${targetCount > 1 ? 's' : ''} to Proceed`;
        proceedBtn.style.opacity = '0.6';
        proceedBtn.style.cursor = 'not-allowed';
      } else {
        const seatCodes = selected.map(s => s.seatCode).join(', ');
        let typeStr = '';
        if (vipCount > 0 && regCount > 0) {
          typeStr = ` (${vipCount} VIP, ${regCount} Reg)`;
        } else if (vipCount > 0) {
          typeStr = ` (${vipCount} VIP)`;
        } else if (regCount > 0) {
          typeStr = ` (${regCount} Reg)`;
        }
        seatVal.textContent = `${seatCodes}${typeStr}`;
        priceVal.textContent = formatPrice(grandTotal);

        if (count === targetCount) {
          proceedBtn.disabled = false;
          proceedBtn.textContent = `Proceed to Pay (${count} Seat${count > 1 ? 's' : ''}) →`;
          proceedBtn.style.opacity = '1';
          proceedBtn.style.cursor = 'pointer';
        } else {
          proceedBtn.disabled = true;
          proceedBtn.textContent = `Select ${remaining} more seat${remaining > 1 ? 's' : ''}`;
          proceedBtn.style.opacity = '0.6';
          proceedBtn.style.cursor = 'not-allowed';
        }
      }
    }
  }

  container.querySelectorAll('.seat-btn:not(.booked)').forEach(btn => {
    btn.addEventListener('click', () => {
      const rowChar = btn.dataset.row;
      const colNum = Number(btn.dataset.col);
      const isVip = (rowChar === 'A' || rowChar === 'B');
      const price = isVip ? vipPrice : regPrice;
      const seatCode = `${rowChar}${colNum}`;

      const existingIndex = APP_STATE.selectedSeats.findIndex(s => s.row === rowChar && s.col === colNum);
      if (existingIndex >= 0) {
        APP_STATE.selectedSeats.splice(existingIndex, 1);
        btn.classList.remove('selected');
      } else {
        if (APP_STATE.selectedSeats.length >= targetCount) {
          showToast(`You have already selected all ${targetCount} seats. Click a selected seat to deselect it.`, 'info');
          return;
        }
        APP_STATE.selectedSeats.push({
          row: rowChar,
          col: colNum,
          seatCode: seatCode,
          isVip: isVip,
          type: isVip ? 'VIP Recliner' : 'Executive Regular',
          price: price
        });
        btn.classList.add('selected');
      }

      updateCheckoutUI();
    });
  });

  const btnProceed = container.querySelector('#btn-proceed-book');
  if (btnProceed) {
    btnProceed.addEventListener('click', () => {
      if (APP_STATE.selectedSeats.length !== targetCount) {
        showToast(`Please select exactly ${targetCount} seats before proceeding.`, 'warning');
        return;
      }
      openBookingModal(eventObj, APP_STATE.selectedSeats);
    });
  }
}

function renderSingleRowHTML(rowChar, seatsArray, tierType) {
  const rowSeats = seatsArray.filter(s => s.rowChar === rowChar || s.row === (rowChar.charCodeAt(0) - 64));

  let colsHTML = '';
  for (let c = 1; c <= 12; c++) {
    const seatObj = rowSeats.find(s => s.col === c);
    const isBooked = seatObj ? seatObj.booked : false;
    const isVip = (rowChar === 'A' || rowChar === 'B');

    let cls = isBooked ? 'seat-btn booked' : (isVip ? 'seat-btn available-vip' : 'seat-btn available-reg');
    colsHTML += `<button class="${cls}" data-row="${rowChar}" data-col="${c}" title="Seat ${rowChar}${c} (${isVip ? 'VIP' : 'Regular'}${isBooked ? ' - Booked' : ''})">${c}</button>`;
    if (c === 4 || c === 8) {
      colsHTML += `<div class="col-gap"></div>`;
    }
  }

  return `
    <div class="seat-row">
      <div class="row-label">${rowChar}</div>
      <div class="seat-cols-wrap">${colsHTML}</div>
      <div class="row-label">${rowChar}</div>
    </div>`;
}

function openBookingModal(eventObj, selectedSeatsArray) {
  const vipPrice = eventObj.vip_price || eventObj.vipPrice;
  const regPrice = eventObj.regular_price || eventObj.regularPrice;
  const vipSeats = selectedSeatsArray.filter(s => s.isVip);
  const regSeats = selectedSeatsArray.filter(s => !s.isVip);
  const vipTotal = vipSeats.length * vipPrice;
  const regTotal = regSeats.length * regPrice;
  const grandTotal = vipTotal + regTotal;
  const totalSeatsCount = selectedSeatsArray.length;

  const seatBadgesHTML = selectedSeatsArray.map(s => `
    <span class="seat-code-pill ${s.isVip ? 'vip' : 'reg'}">${s.seatCode} (${s.isVip ? 'VIP' : 'Reg'})</span>
  `).join('');

  openModal({
    title: `Complete Reservation (${totalSeatsCount} Seat${totalSeatsCount > 1 ? 's' : ''})`,
    bodyHTML: `
      <div style="background:#f8f9fa;border-radius:8px;padding:14px;margin-bottom:18px;border:1px solid #e5e7eb;">
        <div style="font-size:15px;font-weight:700;color:var(--bms-text-main);">${esc(eventObj.title)}</div>
        <div style="font-size:12px;color:var(--bms-text-muted);margin-top:2px;">${esc(eventObj.venue)} &bull; ${esc(eventObj.time)}</div>
        
        <div style="margin-top:10px;padding-top:10px;border-top:1px dashed #d1d5db;">
          <div style="font-size:12px;color:var(--bms-text-muted);margin-bottom:4px;font-weight:600;">Selected Seats (${totalSeatsCount}):</div>
          <div>${seatBadgesHTML}</div>
        </div>

        <table class="breakdown-table">
          <thead>
            <tr>
              <th>Ticket Type</th>
              <th style="text-align:center;">Qty</th>
              <th style="text-align:right;">Subtotal</th>
            </tr>
          </thead>
          <tbody>
            ${vipSeats.length ? `
              <tr>
                <td>VIP Recliner (${formatPrice(vipPrice)})</td>
                <td style="text-align:center;">${vipSeats.length}</td>
                <td style="text-align:right;">${formatPrice(vipTotal)}</td>
              </tr>
            ` : ''}
            ${regSeats.length ? `
              <tr>
                <td>Executive Regular (${formatPrice(regPrice)})</td>
                <td style="text-align:center;">${regSeats.length}</td>
                <td style="text-align:right;">${formatPrice(regTotal)}</td>
              </tr>
            ` : ''}
            <tr class="total-row">
              <td colspan="2">Total Amount (${totalSeatsCount} Seats)</td>
              <td style="text-align:right;color:var(--bms-primary);">${formatPrice(grandTotal)}</td>
            </tr>
          </tbody>
        </table>
      </div>

      <div class="form-group">
        <label>Attendee / Buyer Name *</label>
        <input type="text" id="buyer-name-input" placeholder="Enter full name for booking pass" autofocus autocomplete="off" />
      </div>
      <p style="font-size:11.5px;color:var(--bms-text-muted);line-height:1.4;">
        Direct C99 Backend Atomic Transaction: All seats verified and allocated together. Persisted to binary <code>.dat</code> storage.
      </p>`,
    actions: [
      { id: 'cancel', label: 'Cancel', cls: 'btn-ghost', onClick: closeModal },
      {
        id: 'confirm',
        label: `Confirm & Pay ${formatPrice(grandTotal)}`,
        cls: 'btn-primary',
        onClick: async () => {
          const name = document.getElementById('buyer-name-input').value.trim();
          if (!name) {
            showToast('Please enter the attendee name.', 'error');
            return;
          }

          const payload = {
            eventId: eventObj.id,
            buyerName: name,
            seats: selectedSeatsArray.map(s => ({ row: s.row, col: s.col }))
          };

          const res = await api('/book', 'POST', payload);
          closeModal();

          if (res && res.success) {
            showToast(`🎉 ${res.message || 'Tickets successfully booked and persisted to C data files!'}`, 'success');
            await loadAllData();

            APP_STATE.recentTicket = {
              bookingId: res.bookingId || (res.bookingIds && res.bookingIds[0]) || res.id,
              bookingIds: res.bookingIds || (res.bookingId ? [res.bookingId] : []),
              buyerName: name,
              eventTitle: eventObj.title,
              artist: eventObj.artist,
              venue: eventObj.venue,
              time: eventObj.time,
              seats: selectedSeatsArray,
              totalPrice: res.totalPrice || grandTotal,
              category: eventObj.category,
              breakdown: {
                vipCount: vipSeats.length,
                regCount: regSeats.length,
                vipPrice: vipPrice,
                regPrice: regPrice
              }
            };

            APP_STATE.selectedSeats = [];
            APP_STATE.currentView = 'ticket-pass';
            renderApp();
          } else {
            showToast((res && res.message) ? res.message : 'Booking failed.', 'error');
          }
        }
      }
    ]
  });

  setTimeout(() => {
    const inp = document.getElementById('buyer-name-input');
    if (inp) inp.focus();
  }, 100);
}

function openWaitlistModal(eventId) {
  const ev = APP_STATE.events.find(e => e.id === eventId);
  if (!ev) return;

  openModal({
    title: 'Join FIFO Waiting List Queue',
    bodyHTML: `
      <div style="background:#fef2f2;border-radius:8px;padding:14px;margin-bottom:16px;border:1px solid #fecaca;">
        <strong style="color:var(--bms-danger);font-size:14px;">Housefull — ${esc(ev.title)}</strong>
        <p style="font-size:12px;color:#7f1d1d;margin-top:4px;">
          All seats are currently booked. Join the FIFO (First-In, First-Out) waiting queue. When any ticket is cancelled, the C engine automatically reallocates the freed seat to the next waiting customer!
        </p>
      </div>
      <div class="form-group">
        <label>Your Full Name *</label>
        <input type="text" id="wl-buyer-name" placeholder="Enter attendee name" autocomplete="off" />
      </div>`,
    actions: [
      { id: 'cancel', label: 'Close', cls: 'btn-ghost', onClick: closeModal },
      {
        id: 'join',
        label: 'Enlist in Queue',
        cls: 'btn-primary',
        onClick: async () => {
          const name = document.getElementById('wl-buyer-name').value.trim();
          if (!name) {
            showToast('Please enter your name.', 'error');
            return;
          }

          const res = await api('/waitlist', 'POST', { eventId: ev.id, buyerName: name });
          closeModal();

          if (res && res.success) {
            showToast(`🎉 Enlisted in FIFO Queue! Token #${res.token || ''}`, 'success');
            await loadAllData();
            renderApp();
          } else {
            showToast((res && res.message) ? res.message : 'Could not join waiting list.', 'error');
          }
        }
      }
    ]
  });

  setTimeout(() => {
    const inp = document.getElementById('wl-buyer-name');
    if (inp) inp.focus();
  }, 100);
}

function renderTicketPassView(container, ticket) {
  if (!ticket) {
    APP_STATE.currentView = 'home';
    renderApp();
    return;
  }

  const seatsList = ticket.seats || [];
  const seatCodes = seatsList.length
    ? seatsList.map(s => s.seatCode || `${s.row}${s.col}`).join(', ')
    : `${ticket.row || ''}${ticket.col || ''}`;
  const totalCount = seatsList.length || 1;
  const bookingIdsStr = (ticket.bookingIds && ticket.bookingIds.length)
    ? ticket.bookingIds.map(id => `#${id}`).join(', ')
    : `#${ticket.bookingId}`;

  container.innerHTML = `
    <div class="ticket-pass-wrapper">
      <div style="text-align:center;margin-bottom:16px;">
        <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="#e8192c" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" style="margin:0 auto 8px;display:block;"><path d="M20 12v7a1 1 0 0 1-1 1H5a1 1 0 0 1-1-1v-7"/><rect x="2" y="7" width="20" height="5" rx="1"/><path d="M12 7V3"/><path d="M9 7V5"/><path d="M15 7V5"/></svg>
        <h2 style="font-size:20px;font-weight:800;color:var(--bms-text-main);margin-top:4px;">Booking Confirmed!</h2>
        <p style="font-size:13px;color:var(--bms-text-muted);">Here is your digital boarding pass &amp; ticket stub</p>
      </div>

      <div class="ticket-pass-card">
        <div class="ticket-header-strip">
          <div class="ticket-logo">SHOWSPOT PASS</div>
          <div class="ticket-status">CONFIRMED &bull; VALID (${totalCount} SEAT${totalCount > 1 ? 'S' : ''})</div>
        </div>

        <div class="ticket-body">
          <div class="ticket-event-title">${esc(ticket.eventTitle)}</div>
          <div class="ticket-artist">Presented by ${esc(ticket.artist || 'Production Troupe')}</div>

          <div class="ticket-details-grid">
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Booking ID(s)</span>
              <span class="ticket-detail-val" style="font-size:13px;">${bookingIdsStr}</span>
            </div>
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Seats Assigned (${totalCount})</span>
              <span class="ticket-detail-val seat" style="font-size:16px;">${seatCodes}</span>
            </div>
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Attendee Name</span>
              <span class="ticket-detail-val">${esc(ticket.buyerName)}</span>
            </div>
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Total Amount Paid</span>
              <span class="ticket-detail-val">${formatPrice(ticket.totalPrice || ticket.price)}</span>
            </div>
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Show Venue</span>
              <span class="ticket-detail-val">${esc(ticket.venue || 'Main Stage')}</span>
            </div>
            <div class="ticket-detail-item">
              <span class="ticket-detail-label">Show Time</span>
              <span class="ticket-detail-val">${esc(ticket.time || 'Evening')}</span>
            </div>
          </div>
        </div>

        <div class="ticket-perforation"></div>

        <div class="ticket-barcode-section">
          <div class="ticket-barcode">|||| | |||||| || | |||| |||</div>
          <div class="ticket-booking-id-text">TRANSACTION ID: SS-${ticket.bookingIds ? ticket.bookingIds.join('-') : ticket.bookingId}-C99</div>
        </div>

        <div class="ticket-actions-bar">
          <button class="btn-ghost" id="btn-ticket-home">← Back to Shows</button>
          <button class="btn-primary" id="btn-ticket-print" onclick="window.print()">Print Ticket</button>
        </div>
      </div>
    </div>`;

  const btnHome = container.querySelector('#btn-ticket-home');
  if (btnHome) {
    btnHome.addEventListener('click', () => {
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }
}

function renderMyBookingsView(container) {
  const bookings = APP_STATE.bookings;

  container.innerHTML = `
    <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:20px;flex-wrap:wrap;gap:12px;">
      <div>
        <h2 style="font-size:20px;font-weight:800;color:var(--bms-text-main);">Active Ticket Bookings</h2>
        <p style="font-size:13px;color:var(--bms-text-muted);">Manage your reservations, view receipts or cancel tickets</p>
      </div>
      <button class="btn-ghost" id="btn-back-from-bookings">← Back to Shows</button>
    </div>

    ${!bookings.length ? `
      <div style="background:#ffffff;border-radius:12px;padding:48px;text-align:center;border:1px solid #e5e7eb;">
        <div style="width:52px;height:52px;border-radius:50%;background:#f3f4f6;display:flex;align-items:center;justify-content:center;margin:0 auto 16px;color:var(--bms-text-muted);">
          <svg width="26" height="26" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"><path d="M20 12v7a1 1 0 0 1-1 1H5a1 1 0 0 1-1-1v-7"/><rect x="2" y="7" width="20" height="5" rx="1"/></svg>
        </div>
        <h3 style="font-size:18px;font-weight:700;color:var(--bms-text-main);margin-bottom:6px;">No Active Bookings Found</h3>
        <p style="font-size:13px;color:var(--bms-text-muted);margin-bottom:18px;">Browse our commercial movies and regional theatre productions to book your tickets.</p>
        <button class="btn-primary" id="btn-empty-book-now">Discover Shows Now</button>
      </div>` : `
      <div class="bookings-table-card">
        <table class="modern-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>Attendee</th>
              <th>Event Title</th>
              <th>Seat Code</th>
              <th>Amount</th>
              <th>Action</th>
            </tr>
          </thead>
          <tbody>
            ${bookings.map(b => `
              <tr>
                <td><strong>#${b.id || b.booking_id || b.bookingId}</strong></td>
                <td>${esc(b.buyer_name || b.buyerName)}</td>
                <td><strong>${esc(b.event_title || b.eventTitle)}</strong></td>
                <td><span style="background:#f1f5f9;padding:4px 8px;border-radius:4px;font-weight:700;color:var(--bms-text-main);">${b.row}${b.col}</span></td>
                <td><strong>${formatPrice(b.price)}</strong></td>
                <td>
                  <button class="btn-cancel-ticket" data-cancel-id="${b.id || b.booking_id || b.bookingId}" data-cancel-name="${esc(b.buyer_name || b.buyerName)}">
                    Cancel Ticket
                  </button>
                </td>
              </tr>
            `).join('')}
          </tbody>
        </table>
      </div>
    `}`;

  const btnBack = container.querySelector('#btn-back-from-bookings');
  if (btnBack) {
    btnBack.addEventListener('click', () => {
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  const btnEmpty = container.querySelector('#btn-empty-book-now');
  if (btnEmpty) {
    btnEmpty.addEventListener('click', () => {
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  container.querySelectorAll('[data-cancel-id]').forEach(btn => {
    btn.addEventListener('click', () => {
      const bid = Number(btn.dataset.cancelId);
      const bname = btn.dataset.cancelName;
      confirmCancelBooking(bid, bname);
    });
  });
}

function confirmCancelBooking(bookingId, buyerName) {
  openModal({
    title: 'Confirm Ticket Cancellation',
    bodyHTML: `
      <p style="font-size:14px;color:var(--bms-text-main);margin-bottom:12px;">
        Are you sure you want to cancel booking <strong>#${bookingId}</strong> for <strong>${esc(buyerName)}</strong>?
      </p>
      <div style="background:#fef2f2;border:1px solid #fee2e2;border-radius:6px;padding:12px;font-size:12px;color:#991b1b;line-height:1.5;">
        <strong>LIFO Undo Stack &amp; FIFO Queue:</strong><br>
        1. This cancellation is pushed to the C LIFO undo stack.<br>
        2. If attendees are on the event's FIFO waiting list, this seat is instantly auto-allocated to them.<br>
        3. You can undo this cancellation at any time using the <strong>Undo</strong> button.
      </div>`,
    actions: [
      { id: 'keep', label: 'Keep Ticket', cls: 'btn-ghost', onClick: closeModal },
      {
        id: 'do-cancel',
        label: 'Confirm Cancellation',
        cls: 'btn-primary',
        onClick: async () => {
          closeModal();
          const res = await api('/cancel', 'POST', { bookingId: bookingId });
          if (res && res.success) {
            let msg = res.message || 'Ticket cancelled successfully.';
            if (res.waitlistAllocated || res.autoAllocated) {
              msg = 'Ticket cancelled. Seat was automatically reallocated to waiting list queue!';
            }
            showToast(msg, 'success');
            await loadAllData();
            renderApp();
          } else {
            showToast((res && res.message) ? res.message : 'Cancellation failed.', 'error');
          }
        }
      }
    ]
  });
}

async function triggerUndoAction() {
  const res = await api('/undo', 'POST', {});
  if (res && res.success) {
    showToast(`↩️ ${res.message || 'Last cancellation successfully undone!'}`, 'success');
    await loadAllData();
    renderApp();
  } else {
    showToast((res && res.message) ? res.message : 'No cancellations available on undo stack.', 'warning');
  }
}

function renderArtistView(container) {
  const regionalEvents = APP_STATE.events.filter(e => e.category >= 2);

  container.innerHTML = `
    <div class="portal-dashboard-header">
      <div class="portal-header-info">
        <h2>Artist &amp; Regional Troupe Portal</h2>
        <p>List indigenous theatre, folk dance, Sufi music and regional stage performances directly into the C99 engine.</p>
      </div>
      <button class="btn-ghost" id="btn-exit-artist-portal" style="background:rgba(255,255,255,0.1);color:#ffffff;border-color:rgba(255,255,255,0.3);">
        ← Return to Buyer Homepage
      </button>
    </div>

    <div class="form-panel-card">
      <h3 class="form-panel-title">Register a Regional / Local Cultural Event</h3>
      <form id="artist-reg-form">
        <div class="form-grid-2">
          <div class="form-group">
            <label>Event Category *</label>
            <select id="art-category" required>
              <option value="2">Regional Theatre / Drama</option>
              <option value="3">Local Music Performance</option>
              <option value="4">Folk &amp; Traditional Dance</option>
              <option value="5">Cultural Event / Festival</option>
            </select>
          </div>
          <div class="form-group">
            <label>Event Title *</label>
            <input type="text" id="art-title" placeholder="e.g. Shakuntala Musical Play" required />
          </div>
        </div>

        <div class="form-grid-2">
          <div class="form-group">
            <label>Artist / Troupe Name *</label>
            <input type="text" id="art-artist" placeholder="e.g. Kalidas Rangmanch Troupe" required />
          </div>
          <div class="form-group">
            <label>Auditorium / Venue *</label>
            <input type="text" id="art-venue" placeholder="e.g. Sahitya Kala Auditorium" required />
          </div>
        </div>

        <div class="form-grid-2">
          <div class="form-group">
            <label>Show Schedule / Time *</label>
            <input type="text" id="art-time" placeholder="e.g. 06:30 PM" required />
          </div>
          <div class="form-group">
            <label>Regular Ticket Price (₹) *</label>
            <input type="number" id="art-regular" placeholder="e.g. 150" min="1" step="0.5" required />
          </div>
        </div>

        <div class="form-grid-2">
          <div class="form-group">
            <label>VIP Recliner Price (₹) *</label>
            <input type="number" id="art-vip" placeholder="e.g. 280" min="1" step="0.5" required />
          </div>
          <div class="form-group" style="display:flex;justify-content:flex-end;align-items:flex-end;">
            <button type="submit" class="btn-primary" style="width:100%;padding:12px;">Publish Event to Platform</button>
          </div>
        </div>
      </form>
    </div>

    <section class="showcase-section">
      <div class="section-header">
        <div class="section-title-wrap">
          <h2 class="section-title">Active Regional &amp; Cultural Shows (${regionalEvents.length})</h2>
          <span class="section-subtitle">Live in memory across C singly linked list nodes</span>
        </div>
      </div>
      ${renderCardGridHTML(regionalEvents)}
    </section>`;

  const btnExit = container.querySelector('#btn-exit-artist-portal');
  if (btnExit) {
    btnExit.addEventListener('click', () => {
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  const form = container.querySelector('#artist-reg-form');
  if (form) {
    form.addEventListener('submit', async e => {
      e.preventDefault();
      const payload = {
        category: Number(document.getElementById('art-category').value),
        title: document.getElementById('art-title').value.trim(),
        artist: document.getElementById('art-artist').value.trim(),
        venue: document.getElementById('art-venue').value.trim(),
        time: document.getElementById('art-time').value.trim(),
        regular_price: Number(document.getElementById('art-regular').value),
        vip_price: Number(document.getElementById('art-vip').value)
      };

      const res = await api('/events', 'POST', payload);
      if (res && res.success) {
        showToast(`🎉 "${payload.title}" registered successfully! Added to linked list and persisted to disk.`, 'success');
        form.reset();
        await loadAllData();
        renderApp();
      } else {
        showToast((res && res.message) ? res.message : 'Could not register event.', 'error');
      }
    });
  }

  wireCardButtons(container);
}

function renderOfficerView(container) {
  const ov = APP_STATE.overview || {};
  const totalEvents = ov.total_events || ov.totalEvents || APP_STATE.events.length;
  const totalBookings = ov.total_bookings || ov.totalBookings || APP_STATE.bookings.length;
  const totalRevenue = ov.total_revenue || ov.totalRevenue || 0;
  const seatsOccupied = ov.seats_occupied || ov.seatsOccupied || 0;
  const waitlistEntries = ov.waitlist_entries || ov.waitlistEntries || 0;
  const undoDepth = ov.undoStackDepth !== undefined ? ov.undoStackDepth : 0;

  container.innerHTML = `
    <div class="portal-dashboard-header">
      <div class="portal-header-info">
        <h2>Nodal Officer Oversight Dashboard</h2>
        <p>Comprehensive system health, revenue analytics, audit logs &amp; commercial movie listings.</p>
      </div>
      <button class="btn-ghost" id="btn-exit-officer-portal" style="background:rgba(255,255,255,0.1);color:#ffffff;border-color:rgba(255,255,255,0.3);">
        ← Return to Buyer Homepage
      </button>
    </div>

    <!-- KPI Metric Cards -->
    <div class="kpi-grid">
      <div class="kpi-card">
        <div class="kpi-icon-box red">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="6" width="20" height="12" rx="2"/><circle cx="6" cy="12" r="2" fill="currentColor"/><path d="M10 6v12M14 6v12"/></svg>
        </div>
        <div class="kpi-content">
          <span class="kpi-value">${totalEvents}</span>
          <span class="kpi-label">Total Shows Listed</span>
        </div>
      </div>
      <div class="kpi-card">
        <div class="kpi-icon-box blue">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/></svg>
        </div>
        <div class="kpi-content">
          <span class="kpi-value">${totalBookings}</span>
          <span class="kpi-label">Active Bookings</span>
        </div>
      </div>
      <div class="kpi-card">
        <div class="kpi-icon-box green">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" y1="1" x2="12" y2="23"/><path d="M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>
        </div>
        <div class="kpi-content">
          <span class="kpi-value">${formatPrice(totalRevenue)}</span>
          <span class="kpi-label">Gross Revenue</span>
        </div>
      </div>
      <div class="kpi-card">
        <div class="kpi-icon-box gold">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20.59 13.41l-7.17 7.17a2 2 0 0 1-2.83 0L2 12V2h10l8.59 8.59a2 2 0 0 1 0 2.82z"/><line x1="7" y1="7" x2="7.01" y2="7"/></svg>
        </div>
        <div class="kpi-content">
          <span class="kpi-value">${seatsOccupied}</span>
          <span class="kpi-label">Seats Reserved</span>
        </div>
      </div>
      <div class="kpi-card">
        <div class="kpi-icon-box purple">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
        </div>
        <div class="kpi-content">
          <span class="kpi-value">${waitlistEntries}</span>
          <span class="kpi-label">FIFO Waitlist Entries</span>
        </div>
      </div>
    </div>

    <!-- Add Commercial Movie -->
    <div class="form-panel-card">
      <h3 class="form-panel-title">Add Commercial Movie Screening</h3>
      <form id="officer-movie-form">
        <div class="form-grid-2">
          <div class="form-group">
            <label>Movie Title *</label>
            <input type="text" id="mov-title" placeholder="e.g. Dune: Part Two" required />
          </div>
          <div class="form-group">
            <label>Studio / Distributor *</label>
            <input type="text" id="mov-artist" placeholder="e.g. Legendary / Warner Bros" required />
          </div>
        </div>
        <div class="form-grid-2">
          <div class="form-group">
            <label>Multiplex / Cinema Venue *</label>
            <input type="text" id="mov-venue" placeholder="e.g. INOX Megaplex, Screen 4" required />
          </div>
          <div class="form-group">
            <label>Show Time *</label>
            <input type="text" id="mov-time" placeholder="e.g. 09:15 PM" required />
          </div>
        </div>
        <div class="form-grid-2">
          <div class="form-group">
            <label>Regular Price (Rows C-J) in ₹ *</label>
            <input type="number" id="mov-regular" placeholder="e.g. 220" min="1" step="0.5" required />
          </div>
          <div class="form-group">
            <label>VIP Recliner Price (Rows A-B) in ₹ *</label>
            <input type="number" id="mov-vip" placeholder="e.g. 380" min="1" step="0.5" required />
          </div>
        </div>
        <div class="form-actions-row">
          <button type="submit" class="btn-primary" style="padding:10px 24px;">Publish Commercial Screening</button>
        </div>
      </form>
    </div>

    <!-- Active Bookings Audit Log -->
    <div class="form-panel-card">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:16px;">
        <h3 class="form-panel-title" style="margin-bottom:0;">System Audit Log — All Active Bookings</h3>
        <span style="font-size:12px;color:var(--bms-text-muted);">LIFO Stack Undo Depth: <strong>${undoDepth}</strong></span>
      </div>
      <div class="bookings-table-card">
        <table class="modern-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>Attendee</th>
              <th>Show Title</th>
              <th>Seat Code</th>
              <th>Amount</th>
              <th>Admin Override</th>
            </tr>
          </thead>
          <tbody>
            ${APP_STATE.bookings.map(b => `
              <tr>
                <td><strong>#${b.id || b.booking_id || b.bookingId}</strong></td>
                <td>${esc(b.buyer_name || b.buyerName)}</td>
                <td>${esc(b.event_title || b.eventTitle)}</td>
                <td><span style="background:#f1f5f9;padding:3px 8px;border-radius:4px;font-weight:700;">${b.row}${b.col}</span></td>
                <td><strong>${formatPrice(b.price)}</strong></td>
                <td>
                  <button class="btn-cancel-ticket" data-admin-cancel="${b.id || b.booking_id || b.bookingId}">Cancel Booking</button>
                </td>
              </tr>
            `).join('')}
          </tbody>
        </table>
      </div>
    </div>`;

  const btnExit = container.querySelector('#btn-exit-officer-portal');
  if (btnExit) {
    btnExit.addEventListener('click', () => {
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  const form = container.querySelector('#officer-movie-form');
  if (form) {
    form.addEventListener('submit', async e => {
      e.preventDefault();
      const payload = {
        category: 1,
        title: document.getElementById('mov-title').value.trim(),
        artist: document.getElementById('mov-artist').value.trim(),
        venue: document.getElementById('mov-venue').value.trim(),
        time: document.getElementById('mov-time').value.trim(),
        regular_price: Number(document.getElementById('mov-regular').value),
        vip_price: Number(document.getElementById('mov-vip').value)
      };

      const res = await api('/events', 'POST', payload);
      if (res && res.success) {
        showToast(`🎬 "${payload.title}" added to commercial listings and persisted!`, 'success');
        form.reset();
        await loadAllData();
        renderApp();
      } else {
        showToast((res && res.message) ? res.message : 'Could not add movie.', 'error');
      }
    });
  }

  container.querySelectorAll('[data-admin-cancel]').forEach(btn => {
    btn.addEventListener('click', () => {
      const bid = Number(btn.dataset.adminCancel);
      confirmCancelBooking(bid, 'Attendee');
    });
  });
}

function updateCategoryTabs() {
  document.querySelectorAll('.cat-tab').forEach(tab => {
    tab.classList.toggle('active', tab.dataset.category === APP_STATE.selectedCategory);
  });
}

function initDOMListeners() {
  const brand = document.getElementById('nav-brand');
  if (brand) {
    brand.addEventListener('click', e => {
      e.preventDefault();
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'home';
      APP_STATE.selectedCategory = 'all';
      APP_STATE.searchQuery = '';
      const inp = document.getElementById('global-search-input');
      if (inp) inp.value = '';
      updateCategoryTabs();
      renderApp();
    });
  }

  const searchInput = document.getElementById('global-search-input');
  const searchClear = document.getElementById('search-clear-btn');

  if (searchInput) {
    searchInput.addEventListener('input', e => {
      APP_STATE.searchQuery = e.target.value;
      if (searchClear) searchClear.style.display = APP_STATE.searchQuery ? 'block' : 'none';
      if (APP_STATE.currentView !== 'home') APP_STATE.currentView = 'home';
      renderApp();
    });
  }

  if (searchClear) {
    searchClear.addEventListener('click', () => {
      if (searchInput) searchInput.value = '';
      APP_STATE.searchQuery = '';
      searchClear.style.display = 'none';
      renderApp();
    });
  }

  const locPicker = document.getElementById('location-picker');
  const cityDrop = document.getElementById('city-dropdown');
  const cityDisplay = document.getElementById('current-city');

  if (locPicker && cityDrop) {
    locPicker.addEventListener('click', e => {
      e.stopPropagation();
      cityDrop.classList.toggle('show');
    });

    document.querySelectorAll('.city-pill').forEach(pill => {
      pill.addEventListener('click', e => {
        e.stopPropagation();
        document.querySelectorAll('.city-pill').forEach(p => p.classList.remove('active'));
        pill.classList.add('active');
        APP_STATE.currentCity = pill.dataset.city;
        if (cityDisplay) cityDisplay.textContent = APP_STATE.currentCity;
        cityDrop.classList.remove('show');
        showToast(`Location set to ${APP_STATE.currentCity}`, 'info');
        renderApp();
      });
    });

    document.addEventListener('click', () => {
      cityDrop.classList.remove('show');
    });
  }

  const btnBuyer = document.getElementById('nav-buyer-btn');
  const btnArtist = document.getElementById('nav-artist-btn');
  const btnOfficer = document.getElementById('nav-officer-btn');

  if (btnBuyer) {
    btnBuyer.addEventListener('click', () => {
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'home';
      renderApp();
    });
  }
  if (btnArtist) {
    btnArtist.addEventListener('click', () => {
      APP_STATE.currentPortal = 'artist';
      renderApp();
    });
  }
  if (btnOfficer) {
    btnOfficer.addEventListener('click', () => {
      APP_STATE.currentPortal = 'officer';
      renderApp();
    });
  }

  const quickBookings = document.getElementById('quick-bookings-btn');
  if (quickBookings) {
    quickBookings.addEventListener('click', () => {
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'bookings';
      renderApp();
    });
  }

  const quickUndo = document.getElementById('quick-undo-btn');
  if (quickUndo) {
    quickUndo.addEventListener('click', () => {
      triggerUndoAction();
    });
  }

  document.querySelectorAll('.cat-tab').forEach(tab => {
    tab.addEventListener('click', () => {
      APP_STATE.selectedCategory = tab.dataset.category;
      APP_STATE.searchQuery = '';
      const inp = document.getElementById('global-search-input');
      if (inp) inp.value = '';
      if (APP_STATE.currentPortal !== 'buyer') APP_STATE.currentPortal = 'buyer';
      if (APP_STATE.currentView !== 'home') APP_STATE.currentView = 'home';
      updateCategoryTabs();
      renderApp();
    });
  });

  document.querySelectorAll('.footer-cat-link').forEach(link => {
    link.addEventListener('click', e => {
      e.preventDefault();
      APP_STATE.selectedCategory = link.dataset.category;
      APP_STATE.currentPortal = 'buyer';
      APP_STATE.currentView = 'home';
      updateCategoryTabs();
      renderApp();
    });
  });

  document.querySelectorAll('.footer-portal-link').forEach(link => {
    link.addEventListener('click', e => {
      e.preventDefault();
      APP_STATE.currentPortal = link.dataset.portal;
      APP_STATE.currentView = 'home';
      renderApp();
    });
  });
}

async function initApp() {
  initDOMListeners();
  await loadAllData();
  renderApp();
}

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', initApp);
} else {
  initApp();
}
